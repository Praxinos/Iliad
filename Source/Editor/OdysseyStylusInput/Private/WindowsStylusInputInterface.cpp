// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "WindowsStylusInputInterface.h"

#include "Framework/Application/SlateApplication.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Interfaces/IMainFrameModule.h"

#include "WindowsRealTimeStylusPlugin.h"

#if PLATFORM_WINDOWS

class FWindowsStylusInputInterfaceImpl
{
public:
	~FWindowsStylusInputInterfaceImpl();

	TComPtr<IRealTimeStylus> RealTimeStylus;
	TSharedPtr<FWindowsRealTimeStylusPlugin> StylusPlugin;
	void* DLLHandle { nullptr };

    TWeakPtr<SWindow> Window;
    TWeakPtr<SWidget> Widget;
};

FWindowsStylusInputInterface::FWindowsStylusInputInterface(TUniquePtr<FWindowsStylusInputInterfaceImpl> InImpl)
{
	check(InImpl.IsValid());

	Impl = MoveTemp(InImpl);

	// We desire to receive everything, but what we actually will receive is determined in AddTabletContext
	TArray<GUID> DesiredPackets = {
		GUID_PACKETPROPERTY_GUID_X,
		GUID_PACKETPROPERTY_GUID_Y,
		GUID_PACKETPROPERTY_GUID_Z,
		GUID_PACKETPROPERTY_GUID_PACKET_STATUS,
        GUID_PACKETPROPERTY_GUID_TIMER_TICK,
		GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE,
		GUID_PACKETPROPERTY_GUID_TANGENT_PRESSURE,
		GUID_PACKETPROPERTY_GUID_AZIMUTH_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_ALTITUDE_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_X_TILT_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_Y_TILT_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_TWIST_ORIENTATION,
		GUID_PACKETPROPERTY_GUID_WIDTH,
		GUID_PACKETPROPERTY_GUID_HEIGHT,
		// Currently not needed.
		//GUID_PACKETPROPERTY_GUID_BUTTON_PRESSURE,
	};

	Impl->RealTimeStylus->SetDesiredPacketDescription(DesiredPackets.Num(), DesiredPackets.GetData());
}

FWindowsStylusInputInterface::~FWindowsStylusInputInterface() = default;

void FWindowsStylusInputInterface::Tick()
{
	for (const FTabletContextInfo& Context : Impl->StylusPlugin->TabletContexts)
	{
		// don't change focus if the stylus is down
		if ( Context.GetCurrentState().ContainsByPredicate( []( const FStylusState& iStylusState ) { return iStylusState.IsStylusDown(); }) )
		{
			return;
		}
	}

	HANDLE_PTR HCurrentWnd;
	Impl->RealTimeStylus->get_HWND(&HCurrentWnd);

	FSlateApplication& Application = FSlateApplication::Get();

	FWidgetPath WidgetPath = Application.LocateWindowUnderMouse(Application.GetCursorPos(), Application.GetInteractiveTopLevelWindows());
	if (WidgetPath.IsValid())
	{
		TSharedPtr<SWindow> Window = WidgetPath.GetWindow();
		if (Window.IsValid())
		{
			TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow();
			HWND Hwnd = reinterpret_cast<HWND>(NativeWindow->GetOSWindowHandle());

			if (reinterpret_cast<HWND>(HCurrentWnd) != Hwnd)
			{
				// changing the HWND isn't supported when the plugin is enabled
				Impl->RealTimeStylus->put_Enabled(Windows::FALSE);
                FPlatformProcess::Sleep( 0.03 );
				Impl->RealTimeStylus->put_HWND(reinterpret_cast<uint64>(Hwnd));
                FPlatformProcess::Sleep( 0.03 );
				Impl->RealTimeStylus->put_Enabled(Windows::TRUE);
                FPlatformProcess::Sleep( 0.03 );
			}

            Impl->Window = Window;
            Impl->Widget = WidgetPath.GetLastWidget();
		}
	}
}

int32 FWindowsStylusInputInterface::NumInputDevices() const
{
	return Impl->StylusPlugin->TabletContexts.Num();
}

IStylusInputDevice* FWindowsStylusInputInterface::GetInputDevice(int32 Index) const
{
	if (Index < 0 || Index >= Impl->StylusPlugin->TabletContexts.Num())
	{
		return nullptr;
	}

	return &Impl->StylusPlugin->TabletContexts[Index];
}

TWeakPtr<SWindow> FWindowsStylusInputInterface::Window() const
{
    return Impl->Window;
}

TWeakPtr<SWidget> FWindowsStylusInputInterface::Widget() const
{
    return Impl->Widget;
}

FWindowsStylusInputInterfaceImpl::~FWindowsStylusInputInterfaceImpl()
{
	RealTimeStylus->RemoveAllStylusSyncPlugins();
	RealTimeStylus.Reset();

	StylusPlugin.Reset();

	if (DLLHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(DLLHandle);
		DLLHandle = nullptr;
	}
}

TSharedPtr<IStylusInputInterfaceInternal> CreateStylusInputInterface()
{
	if (!FWindowsPlatformMisc::CoInitialize()) 
	{
		UE_LOG(LogStylusInput, Error, TEXT("Could not initialize COM library!"));
		return nullptr;
	}

	TUniquePtr<FWindowsStylusInputInterfaceImpl> WindowsImpl = MakeUnique<FWindowsStylusInputInterfaceImpl>();

	// Load RealTimeStylus DLL
	const FString InkDLLDirectory = TEXT("C:\\Program Files\\Common Files\\microsoft shared\\ink");
	const FString RTSComDLL = TEXT("RTSCom.dll");
	FPlatformProcess::PushDllDirectory(*InkDLLDirectory);

	WindowsImpl->DLLHandle = FPlatformProcess::GetDllHandle(*(InkDLLDirectory / RTSComDLL));
	if (WindowsImpl->DLLHandle == nullptr)
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not load RTSCom.dll!"));
		return nullptr;
	}

	FPlatformProcess::PopDllDirectory(*InkDLLDirectory);

	// Create RealTimeStylus interface
	void* OutInstance { nullptr };
	HRESULT hr = ::CoCreateInstance(__uuidof(RealTimeStylus), nullptr, CLSCTX_INPROC, __uuidof(IRealTimeStylus), &OutInstance);
	if (FAILED(hr))
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not create RealTimeStylus!"));
		return nullptr;
	}

	WindowsImpl->RealTimeStylus = static_cast<IRealTimeStylus*>(OutInstance);
	WindowsImpl->StylusPlugin = MakeShareable(new FWindowsRealTimeStylusPlugin());
	
	// Create free-threaded marshaller for the plugin
	hr = ::CoCreateFreeThreadedMarshaler(WindowsImpl->StylusPlugin.Get(), &WindowsImpl->StylusPlugin->FreeThreadedMarshaller);
	if (FAILED(hr))
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not create FreeThreadedMarshaller!"));
		return nullptr;
	}

	// Add stylus plugin to the interface
	hr = WindowsImpl->RealTimeStylus->AddStylusSyncPlugin(0, WindowsImpl->StylusPlugin.Get());
	if (FAILED(hr))
	{
		FWindowsPlatformMisc::CoUninitialize();
		UE_LOG(LogStylusInput, Error, TEXT("Could not add stylus plugin to API!"));
		return nullptr;
	}
	
	return MakeShared<FWindowsStylusInputInterface>(MoveTemp(WindowsImpl));
}

#endif // PLATFORM_WINDOWS

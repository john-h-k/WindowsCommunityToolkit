//Copyright (c) Microsoft. All rights reserved. Licensed under the MIT license.
//See LICENSE in the project root for license information.

#include "pch.h"
#include "GazeInput.h"
#include "GazePointer.h"
#include "GazePointerProxy.h"
#include "GazeElement.h"

using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Media;

BEGIN_NAMESPACE_GAZE_INPUT

static Brush^ s_progressBrush = ref new SolidColorBrush(Colors::Green);

Brush^ GazeInput::GazeFeedbackProgressBrush::get()
{
    return s_progressBrush;
}

void GazeInput::GazeFeedbackCompleteBrush::set(Brush^ value)
{
    s_progressBrush = value;
}

static Brush^ s_completeBrush = ref new SolidColorBrush(Colors::Red);

Brush^ GazeInput::GazeFeedbackCompleteBrush::get()
{
    return s_completeBrush;
}

void GazeInput::GazeFeedbackProgressBrush::set(Brush^ value)
{
    s_completeBrush = value;
}

TimeSpan GazeInput::UnsetTimeSpan = { -1 };

static void OnIsGazeEnabledChanged(DependencyObject^ ob, DependencyPropertyChangedEventArgs^ args)
{
    auto element = safe_cast<FrameworkElement^>(ob);
    auto isGazeEnabled = safe_cast<GazeEnablement>(args->NewValue);
    GazePointerProxy::SetGazeEnabled(element, isGazeEnabled);
}

static void OnIsCursorVisibleChanged(DependencyObject^ ob, DependencyPropertyChangedEventArgs^ args)
{
    GazePointer::Instance->IsCursorVisible = safe_cast<bool>(args->NewValue);
}

static void OnCursorRadiusChanged(DependencyObject^ ob, DependencyPropertyChangedEventArgs^ args)
{
    GazePointer::Instance->CursorRadius = safe_cast<int>(args->NewValue);
}

static DependencyProperty^ s_isGazeEnabledProperty = DependencyProperty::RegisterAttached("IsGazeEnabled", GazeEnablement::typeid, GazeInput::typeid,
    ref new PropertyMetadata(GazeEnablement::Inherited, ref new PropertyChangedCallback(&OnIsGazeEnabledChanged)));
static DependencyProperty^ s_isCursorVisibleProperty = DependencyProperty::RegisterAttached("IsCursorVisible", bool::typeid, GazeInput::typeid,
    ref new PropertyMetadata(true, ref new PropertyChangedCallback(&OnIsCursorVisibleChanged)));
static DependencyProperty^ s_cursorRadiusProperty = DependencyProperty::RegisterAttached("CursorRadius", int::typeid, GazeInput::typeid,
    ref new PropertyMetadata(6, ref new PropertyChangedCallback(&OnCursorRadiusChanged)));
static DependencyProperty^ s_gazeElementProperty = DependencyProperty::RegisterAttached("GazeElement", GazeElement::typeid, GazeInput::typeid, ref new PropertyMetadata(nullptr));
static DependencyProperty^ s_fixationDurationProperty = DependencyProperty::RegisterAttached("FixationDuration", TimeSpan::typeid, GazeInput::typeid, ref new PropertyMetadata(GazeInput::UnsetTimeSpan));
static DependencyProperty^ s_dwellDurationProperty = DependencyProperty::RegisterAttached("DwellDuration", TimeSpan::typeid, GazeInput::typeid, ref new PropertyMetadata(GazeInput::UnsetTimeSpan));
static DependencyProperty^ s_repeatDelayDurationProperty = DependencyProperty::RegisterAttached("RepeatDelayDuration", TimeSpan::typeid, GazeInput::typeid, ref new PropertyMetadata(GazeInput::UnsetTimeSpan));
static DependencyProperty^ s_dwellRepeatDurationProperty = DependencyProperty::RegisterAttached("DwellRepeatDuration", TimeSpan::typeid, GazeInput::typeid, ref new PropertyMetadata(GazeInput::UnsetTimeSpan));
static DependencyProperty^ s_thresholdDurationProperty = DependencyProperty::RegisterAttached("ThresholdDuration", TimeSpan::typeid, GazeInput::typeid, ref new PropertyMetadata(GazeInput::UnsetTimeSpan));
static DependencyProperty^ s_maxRepeatCountProperty = DependencyProperty::RegisterAttached("MaxDwellRepeatCount", int::typeid, GazeInput::typeid, ref new PropertyMetadata(safe_cast<Object^>(0)));

DependencyProperty^ GazeInput::IsGazeEnabledProperty::get() { return s_isGazeEnabledProperty; }
DependencyProperty^ GazeInput::IsCursorVisibleProperty::get() { return s_isCursorVisibleProperty; }
DependencyProperty^ GazeInput::CursorRadiusProperty::get() { return s_cursorRadiusProperty; }
DependencyProperty^ GazeInput::GazeElementProperty::get() { return s_gazeElementProperty; }
DependencyProperty^ GazeInput::FixationDurationProperty::get() { return s_fixationDurationProperty; }
DependencyProperty^ GazeInput::DwellDurationProperty::get() { return s_dwellDurationProperty; }
DependencyProperty^ GazeInput::RepeatDelayDurationProperty::get() { return s_repeatDelayDurationProperty; }
DependencyProperty^ GazeInput::DwellRepeatDurationProperty::get() { return s_dwellRepeatDurationProperty; }
DependencyProperty^ GazeInput::ThresholdDurationProperty::get() { return s_thresholdDurationProperty; }
DependencyProperty^ GazeInput::MaxDwellRepeatCountProperty::get() { return s_maxRepeatCountProperty; }

GazeEnablement GazeInput::GetIsGazeEnabled(UIElement^ element) { return safe_cast<GazeEnablement>(element->GetValue(s_isGazeEnabledProperty)); }
bool GazeInput::GetIsCursorVisible(UIElement^ element) { return safe_cast<bool>(element->GetValue(s_isCursorVisibleProperty)); }
int GazeInput::GetCursorRadius(UIElement^ element) { return safe_cast<int>(element->GetValue(s_cursorRadiusProperty)); }
GazeElement^ GazeInput::GetGazeElement(UIElement^ element) { return safe_cast<GazeElement^>(element->GetValue(s_gazeElementProperty)); }
TimeSpan GazeInput::GetFixationDuration(UIElement^ element) { return safe_cast<TimeSpan>(element->GetValue(s_fixationDurationProperty)); }
TimeSpan GazeInput::GetDwellDuration(UIElement^ element) { return safe_cast<TimeSpan>(element->GetValue(s_dwellDurationProperty)); }
TimeSpan GazeInput::GetRepeatDelayDuration(UIElement^ element) { return safe_cast<TimeSpan>(element->GetValue(s_repeatDelayDurationProperty)); }
TimeSpan GazeInput::GetDwellRepeatDuration(UIElement^ element) { return safe_cast<TimeSpan>(element->GetValue(s_dwellRepeatDurationProperty)); }
TimeSpan GazeInput::GetThresholdDuration(UIElement^ element) { return safe_cast<TimeSpan>(element->GetValue(s_thresholdDurationProperty)); }
int GazeInput::GetMaxDwellRepeatCount(UIElement^ element) { return safe_cast<int>(element->GetValue(s_maxRepeatCountProperty)); }

void GazeInput::SetIsGazeEnabled(UIElement^ element, GazeEnablement value) { element->SetValue(s_isGazeEnabledProperty, value); }
void GazeInput::SetIsCursorVisible(UIElement^ element, bool value) { element->SetValue(s_isCursorVisibleProperty, value); }
void GazeInput::SetCursorRadius(UIElement^ element, int value) { element->SetValue(s_cursorRadiusProperty, value); }
void GazeInput::SetGazeElement(UIElement^ element, GazeElement^ value) { element->SetValue(s_gazeElementProperty, value); }
void GazeInput::SetFixationDuration(UIElement^ element, TimeSpan span) { element->SetValue(s_fixationDurationProperty, span); }
void GazeInput::SetDwellDuration(UIElement^ element, TimeSpan span) { element->SetValue(s_dwellDurationProperty, span); }
void GazeInput::SetRepeatDelayDuration(UIElement^ element, TimeSpan span) { element->SetValue(s_repeatDelayDurationProperty, span); }
void GazeInput::SetDwellRepeatDuration(UIElement^ element, TimeSpan span) { element->SetValue(s_dwellRepeatDurationProperty, span); }
void GazeInput::SetThresholdDuration(UIElement^ element, TimeSpan span) { element->SetValue(s_thresholdDurationProperty, span); }
void GazeInput::SetMaxDwellRepeatCount(UIElement^ element, int value) { element->SetValue(s_maxRepeatCountProperty, value); }

GazePointer^ GazeInput::GetGazePointer(Page^ page)
{
    return GazePointer::Instance;
}

bool GazeInput::IsDeviceAvailable::get()
{
    return GazePointer::Instance->IsDeviceAvailable; 
}

EventRegistrationToken GazeInput::IsDeviceAvailableChanged::add(EventHandler<Object^>^ handler)
{ 
    return GazePointer::Instance->IsDeviceAvailableChanged += handler; 
}

void GazeInput::IsDeviceAvailableChanged::remove(EventRegistrationToken token) 
{ 
    GazePointer::Instance->IsDeviceAvailableChanged -= token; 
}

END_NAMESPACE_GAZE_INPUT
#include <windows.h>
#include <windowsx.h>

#include "UPlugin/ScriptableBase.h"

using namespace UPlugin;


static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
static WNDPROC lpOldProc = NULL;

ScriptablePluginObjectBase::ScriptablePluginObjectBase(NPP npp)
	:mNpp(npp), m_pWindow(NULL), m_has_window(false), m_use_dotnetsupport(false)
{
}

ScriptablePluginObjectBase::~ScriptablePluginObjectBase()
{
	if (!m_has_window) return;
	if (m_pWindow == NULL || m_pWindow->window == NULL) {
		//TODO @@@ log
		return;
	}
	SubclassWindow((HWND)m_pWindow->window, lpOldProc);
	m_pWindow = NULL;
}

// Set Window
bool ScriptablePluginObjectBase::setWindowBase(NPWindow *pWindow)
{
	if (!m_has_window) return true;
	if (pWindow->window == NULL) return false;
	this->m_pWindow = pWindow;

	// subclass window so we can intercept window messages and
	// do our drawing to it
	lpOldProc = SubclassWindow((HWND)m_pWindow->window, (WNDPROC)PluginWinProc);

	// associate window with our nsPluginInstance object so we can access 
	// it in the window procedure
	SetWindowLongPtr((HWND)m_pWindow->window, GWLP_USERDATA, (LONG_PTR)this);

	// should be overrided
	setWindow(pWindow);

	return true;
}

bool ScriptablePluginObjectBase::setWindow(NPWindow *pWindow)
{
	return true;
}

// Interface Methods
void
ScriptablePluginObjectBase::Invalidate()
{
}

bool
ScriptablePluginObjectBase::HasMethod(NPIdentifier name)
{
  return false;
}

bool
ScriptablePluginObjectBase::Invoke(NPIdentifier name, const NPVariant *args,
                                   uint32_t argCount, NPVariant *result)
{
  return false;
}

bool
ScriptablePluginObjectBase::InvokeDefault(const NPVariant *args,
                                          uint32_t argCount, NPVariant *result)
{
  return false;
}

bool
ScriptablePluginObjectBase::HasProperty(NPIdentifier name)
{
  return false;
}

bool
ScriptablePluginObjectBase::GetProperty(NPIdentifier name, NPVariant *result)
{
  return false;
}

bool
ScriptablePluginObjectBase::SetProperty(NPIdentifier name, const NPVariant *value)
{
  return false;
}

bool
ScriptablePluginObjectBase::RemoveProperty(NPIdentifier name)
{
  return false;
}

bool
ScriptablePluginObjectBase::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
  return false;
}

bool
ScriptablePluginObjectBase::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
  return false;
}

// static
void
ScriptablePluginObjectBase::_Deallocate(NPObject *npobj)
{
  // Call the virtual destructor.
  delete (ScriptablePluginObjectBase *)npobj;
}

// static
void
ScriptablePluginObjectBase::_Invalidate(NPObject *npobj)
{
  ((ScriptablePluginObjectBase *)npobj)->Invalidate();
}

// static
bool
ScriptablePluginObjectBase::_HasMethod(NPObject *npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->HasMethod(name);
}

// static
bool
ScriptablePluginObjectBase::_Invoke(NPObject *npobj, NPIdentifier name,
                                    const NPVariant *args, uint32_t argCount,
                                    NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->Invoke(name, args, argCount,
                                                       result);
}

// static
bool
ScriptablePluginObjectBase::_InvokeDefault(NPObject *npobj,
                                           const NPVariant *args,
                                           uint32_t argCount,
                                           NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->InvokeDefault(args, argCount,
                                                              result);
}

// static
bool
ScriptablePluginObjectBase::_HasProperty(NPObject * npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->HasProperty(name);
}

// static
bool
ScriptablePluginObjectBase::_GetProperty(NPObject *npobj, NPIdentifier name,
                                         NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->GetProperty(name, result);
}

// static
bool
ScriptablePluginObjectBase::_SetProperty(NPObject *npobj, NPIdentifier name,
                                         const NPVariant *value)
{
  return ((ScriptablePluginObjectBase *)npobj)->SetProperty(name, value);
}

// static
bool
ScriptablePluginObjectBase::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->RemoveProperty(name);
}

// static
bool
ScriptablePluginObjectBase::_Enumerate(NPObject *npobj,
                                       NPIdentifier **identifier,
                                       uint32_t *count)
{
  return ((ScriptablePluginObjectBase *)npobj)->Enumerate(identifier, count);
}

// static
bool
ScriptablePluginObjectBase::_Construct(NPObject *npobj, const NPVariant *args,
                                       uint32_t argCount, NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->Construct(args, argCount,
                                                          result);
}


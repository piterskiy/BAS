#include "mainhandler.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "log.h"
#include "match.h"
#include "multithreading.h"

using namespace std::placeholders;

MainHandler::MainHandler()
{
    NeedQuit = false;
    WaitForLoadEvent = false;
    Browser = 0;
    IsVisible = false;
}


void MainHandler::SetData(BrowserData *Data)
{
    this->Data = Data;
}

void MainHandler::SetSettings(settings *Settings)
{
    this->Settings = Settings;
}


CefRefPtr<CefDisplayHandler> MainHandler::GetDisplayHandler()
{
    return this;
}

CefRefPtr<CefLifeSpanHandler> MainHandler::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefLoadHandler> MainHandler::GetLoadHandler()
{
    return this;
}

CefRefPtr<CefRequestHandler> MainHandler::GetRequestHandler()
{
    return this;
}

CefRefPtr<CefDialogHandler> MainHandler::GetDialogHandler()
{
    return this;
}

CefRefPtr<CefKeyboardHandler> MainHandler::GetKeyboardHandler()
{
    return this;
}

CefRefPtr<CefRenderHandler> MainHandler::GetRenderHandler()
{
    return this;
}

CefRefPtr<CefJSDialogHandler> MainHandler::GetJSDialogHandler()
{
    return this;
}

bool MainHandler::OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url, const CefString& accept_lang, JSDialogType dialog_type, const CefString& message_text, const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message)
{
    suppress_message = true;
    return false;
}

bool MainHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,const CefString& message_text, bool is_reload, CefRefPtr<CefJSDialogCallback> callback)
{
    callback->Continue(true,"");
    return true;
}


bool MainHandler::OnKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent& event, CefEventHandle os_event)
{
    worker_log(std::string("CefKeyEvent<<") + std::string("type<<") + std::to_string(event.type)
                + std::string("<<modifiers<<") + std::to_string(event.modifiers)
                + std::string("<<windows_key_code<<") + std::to_string(event.windows_key_code)
                + std::string("<<native_key_code<<") + std::to_string(event.native_key_code)
                + std::string("<<character<<") + std::to_string(event.character)
                + std::string("<<unmodified_character<<") + std::to_string(event.unmodified_character)
                + std::string("<<focus_on_editable_field<<") + std::to_string(event.focus_on_editable_field)
                + std::string("<<is_system_key<<") + std::to_string(event.is_system_key)
               );
    return false;
}

CefRefPtr<CefResourceHandler> MainHandler::GetResourceHandler(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request)
{
    CurlResourceHandler* h = new CurlResourceHandler(Data);
    h->SetForceUtf8(Settings->ForceUtf8());

    EventOnTimerCurlResources.push_back(h);
    CurlResourcesLength = EventOnTimerCurlResources.size();
    return EventOnTimerCurlResources.at(EventOnTimerCurlResources.size() - 1);
}

int MainHandler::GetResourceListLength()
{
    return CurlResourcesLength;
}

void MainHandler::CleanResourceHandlerList()
{
    //Delete expired handlers and run timer on existing
    int i = 0;
    for (std::vector<CefRefPtr<CurlResourceHandler> >::iterator it=EventOnTimerCurlResources.begin();it!=EventOnTimerCurlResources.end();)
    {
        bool CanDelete = it->get()->GetCanDelete();
        if(CanDelete)
        {
            it = EventOnTimerCurlResources.erase(it);
        }
        else
        {
            it->get()->Timer();
            ++it;
        }
        i++;
     }
    CurlResourcesLength = EventOnTimerCurlResources.size();

}


bool MainHandler::GetAuthCredentials(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback)
{
    worker_log(std::string("GetAuthCredentials<<"));
    return false;
}

bool MainHandler::OnCertificateError(CefRefPtr<CefBrowser> browser,cef_errorcode_t cert_error,const CefString& request_url,CefRefPtr<CefSSLInfo> ssl_info,CefRefPtr<CefRequestCallback> callback)
{
    worker_log(std::string("OnCertificateError<<") + request_url.ToString());

    callback->Continue(true);
    return true;
}


bool MainHandler::OnFileDialog(CefRefPtr<CefBrowser> browser, FileDialogMode mode, const CefString& title, const CefString& default_file_path, const std::vector<CefString>& accept_filters, int selected_accept_filter, CefRefPtr<CefFileDialogCallback> callback)
{
    std::vector<CefString> res;
    {
        LOCK_BROWSER_DATA
        res.push_back(Data->_OpenFileName);
    }
    callback->Continue(0,res);
    return true;
}

void MainHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    worker_log("OnTitleChange");

    SetWindowText(Data->_MainWindowHandle, std::wstring(title).c_str());
}

void MainHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    worker_log("OnAfterCreated");

    this->Browser = browser;

    if(IsVisible)
        Show();
}

bool MainHandler::GetIsVisible()
{
    return IsVisible;
}

void MainHandler::Hide()
{
    IsVisible = false;
    //RECT rect;
    //GetWindowRect(Data->MainWindowHandle,&rect);
    ShowWindow(Data->_MainWindowHandle, SW_HIDE);
    //MoveWindow(Data->MainWindowHandle,3000,3000,rect.right-rect.left,rect.bottom-rect.top,true);
}

void MainHandler::Show()
{
    IsVisible = true;
    //RECT rect;
    //GetWindowRect(Data->MainWindowHandle,&rect);
    ShowWindow(Data->_MainWindowHandle, SW_SHOW);
    //MoveWindow(Data->MainWindowHandle,0,0,rect.right-rect.left,rect.bottom-rect.top,true);
    SetForegroundWindow(Data->_MainWindowHandle);
    if(Browser)
        Browser->GetHost()->Invalidate(PET_VIEW);
}

bool MainHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    worker_log("DoClose");
    return false;
}

void MainHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    Browser = 0;
    worker_log("OnBeforeClose");

    for(auto f:EventClosedProgrammatically)
        f();
}

bool MainHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access)
{
    worker_log(std::string("OnBeforePopup<<") + target_url.ToString());
    frame->LoadURL(target_url);
    worker_log(std::string("OnBeforePopup>>"));
    return true;
}

CefRequestHandler::ReturnValue MainHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefRequestCallback> callback)
{
    bool Accept = true;
    std::string url = request->GetURL().ToString();
    {
        LOCK_BROWSER_DATA
        for(std::pair<bool, std::string> p:Data->_RequestMask)
        {
            if(match(p.second,url))
            {
                Accept = p.first;
            }
        }
    }
    if(!Accept || Data->IsReset)
    {
        return RV_CANCEL;
    }

    CefRequest::HeaderMap ReqestHeaders;
    request->GetHeaderMap(ReqestHeaders);

    {
        LOCK_BROWSER_DATA
        for (auto h : Data->_Headers)
        {
            worker_log(std::string("AvailableHeader<<") + h.first);
            ReqestHeaders.erase(h.first);
            ReqestHeaders.insert(std::make_pair(h.first, h.second));
        }
    }

    if(request->GetMethod().ToString() == std::string("POST"))
    {
        CefRefPtr<CefPostData> post = request->GetPostData();
        if(!post.get())
        {
            post = CefPostData::Create();
            request->SetPostData(post);
        }
        if(post->GetElementCount() == 0)
        {
            CefRefPtr<CefPostDataElement> el = CefPostDataElement::Create();
            std::string d("none=");
            el->SetToBytes(d.size(),d.c_str());
            post->AddElement(el);
        }
    }

    request->SetHeaderMap(ReqestHeaders);
    return RV_CONTINUE;
}

void MainHandler::OnResourceLoadComplete(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response, CefRequestHandler::URLRequestStatus status, int64 received_content_length)
{
    for(auto f:EventUrlLoaded)
        f(request->GetURL().ToString(),response->GetStatus());
}




void MainHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
{
    /*worker_log("OnLoadError");

    if (errorCode == ERR_ABORTED)
      return;

    if(frame->IsMain())
    {
        SendTextResponce("<Messages><Load>1</Load></Messages>");
    }*/
}

void MainHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    worker_log(std::string("OnLoadEnd ") + std::to_string(frame->IsMain()) + std::string(" ") + frame->GetURL().ToString() + std::string(" ") + std::to_string(httpStatusCode) );
    if(frame->GetURL().ToString() == "about:blank")
    {
        Data->IsAboutBlankLoaded = true;
        return;
    }

    if(frame->IsMain())
    {
        if(httpStatusCode >= 200 && httpStatusCode < 300)
        {
           browser->GetMainFrame()->ExecuteJavaScript("if(document.body.style['background-color'].length === 0)document.body.style['background-color']='white';", Browser->GetMainFrame()->GetURL(), 0);
           SendTextResponce("<Messages><Load>0</Load></Messages>");
           for(auto f:EventLoadSuccess)
               f();
        }else
        {
            SendTextResponce("<Messages><Load>1</Load></Messages>");
        }
    }
    worker_log("Loaded Data");
}

void MainHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{

}

void MainHandler::SendTextResponce(const std::string& text)
{
    for(auto f:EventSendTextResponce)
        f(text);
}

bool MainHandler::IsNeedQuit()
{
    return NeedQuit;
}

void MainHandler::CloseLastBrowser()
{
    //worker_log(std::string("CloseLastBrowser"));
    //if(!Browser)
        //return;
    //DestroyWindow(Browser->GetHost()->GetWindowHandle());
}

//CefRenderHandler
bool MainHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    worker_log(std::string("GetViewRect<<") + std::to_string(Data->WidthBrowser) + std::string("<<") + std::to_string(Data->HeightBrowser));

    rect.x = 0;
    rect.y = 0;
    rect.width = Data->WidthBrowser;
    rect.height = Data->HeightBrowser;
    return true;
}
void MainHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
{
    if(type == PET_VIEW)
    {
        for(auto f:EventPaint)
            f((char*)buffer, width, height);
        //worker_log(std::string("OnPaint<<") + std::to_string(width) + std::string("<<") + std::to_string(height) + std::string("<<"));
    }
}

void MainHandler::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser,double x,double y)
{
    worker_log(std::string("OFFSET<<") + std::to_string(x) + std::string("<<") + std::to_string(y));
    Data->ScrollX = x;
    Data->ScrollY = y;
}

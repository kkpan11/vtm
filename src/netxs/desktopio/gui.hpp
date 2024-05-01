// Copyright (c) NetXS Group.
// Licensed under the MIT license.

#pragma once

#include <DWrite_2.h>
#pragma comment(lib, "Gdi32")
#pragma comment(lib, "dwrite")

#define ok2(...) [&](){ auto hr = __VA_ARGS__; if (hr != S_OK) log(utf::to_hex(hr), " ", #__VA_ARGS__); return hr == S_OK; }()

namespace netxs::gui
{
    using namespace input;

    auto canvas_text = ansi::wrp(wrap::on).itc(true).fgc(tint::cyanlt).add("\nvtm GUI frontend").itc(faux).fgc(tint::redlt).bld(true).add(" is currently under development.").nil()
        .fgc(tint::cyanlt).add(" You can try it on any versions/editions of Windows platforms starting from Windows 8.1"
                               " (with colored emoji!), including Windows Server Core. 😀😬😁😂😃😄😅😆 👌🐞😎👪.\n\n")
        .fgc(tint::greenlt).add("Press Esc or Right click to close.\n");
    auto header_text = L"Windows Command Prompt - 😎 - C:\\Windows\\System32\\"s;
    auto footer_text = L"4/4000 80:25"s;
    auto canvas_page = ui::page{ canvas_text + canvas_text + canvas_text + canvas_text + canvas_text};
    auto header_para = ui::para{ utf::to_utf(header_text) };
    auto footer_para = ui::para{ utf::to_utf(footer_text) };

    namespace style
    {
        static constexpr auto normal      = 0;
        static constexpr auto italic      = 1;
        static constexpr auto bold        = 2;
        static constexpr auto bold_italic = bold | italic;
    };

    struct alpha_mask
    {
        static constexpr auto undef = 0;
        static constexpr auto plain = 1; // B/W mask.
        static constexpr auto alpha = 2; // Grayscale AA glyph mask.
        static constexpr auto color = 3; // Color AA glyph.

        netxs::raw_vector<byte> bits;
        rect area;
        si32 type{ undef };
    };

    struct surface
    {
        using bits = netxs::raster<std::span<argb>, rect>;

        HDC   hdc;
        argb  fgc;
        HWND hWnd;
        bool sync;
        rect prev;
        rect area;
        twod size;
        bits data;

        surface(surface const&) = default;
        surface(surface&&) = default;
        surface(HWND hWnd)
            :  hdc{ ::CreateCompatibleDC(NULL)}, // Only current thread owns hdc.
              hWnd{ hWnd },
              sync{ faux },
              prev{ .coor = dot_mx },
              area{ dot_00, dot_00 },
              size{ dot_00 }
        { }
        void reset() // We don't use custom copy/move ctors.
        {
            if (hdc) ::DeleteDC(hdc);
        }
        void set_dpi(auto /*dpi*/) // We are do not rely on dpi. Users should configure all metrics in pixels.
        { }
        auto canvas(bool wipe = faux)
        {
            if (area)
            {
                if (area.size != size)
                {
                    auto ptr = (void*)nullptr;
                    auto bmi = BITMAPINFO{ .bmiHeader = { .biSize        = sizeof(BITMAPINFOHEADER),
                                                          .biWidth       = area.size.x,
                                                          .biHeight      = -area.size.y,
                                                          .biPlanes      = 1,
                                                          .biBitCount    = 32,
                                                          .biCompression = BI_RGB }};
                    if (auto hbm = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &ptr, 0, 0)) // 0.050 ms
                    {
                        ::DeleteObject(::SelectObject(hdc, hbm));
                        wipe = faux;
                        size = area.size;
                        data = bits{ std::span<argb>{ (argb*)ptr, (sz_t)size.x * size.y }, rect{ area.coor, size }};
                    }
                    else log("%%Compatible bitmap creation error: %ec%", prompt::gui, ::GetLastError());
                }
                if (wipe) std::memset(data.data(), 0, (sz_t)size.x * size.y * sizeof(argb));
                sync = faux;
            }
            data.move(area.coor);
            return data;
        }
        void present()
        {
            if (sync || !hdc) return;
            //netxs::misc::fill(canvas(), [](argb& c){ c.pma(); });
            static auto blend_props = BLENDFUNCTION{ .BlendOp = AC_SRC_OVER, .SourceConstantAlpha = 255, .AlphaFormat = AC_SRC_ALPHA };
            auto scr_coor = POINT{};
            auto old_size =  SIZE{ prev.size.x, prev.size.y };
            auto old_coor = POINT{ prev.coor.x, prev.coor.y };
            auto win_size =  SIZE{      size.x,      size.y };
            auto win_coor = POINT{ area.coor.x, area.coor.y };
            //auto sized = prev.size(     size);
            auto moved = prev.coor(area.coor);
            auto rc = ::UpdateLayeredWindow(hWnd,   // 1.5 ms (syscall, copy bitmap to hardware)
                                            HDC{},                       // No color palette matching.  HDC hdcDst,
                                            moved ? &win_coor : nullptr, // POINT         *pptDst,
                                            &win_size,                   // SIZE          *psize,
                                            hdc,                         // HDC           hdcSrc,
                                            &scr_coor,                   // POINT         *pptSrc,
                                            {},                          // COLORREF      crKey,
                                            &blend_props,                // BLENDFUNCTION *pblend,
                                            ULW_ALPHA);                  // DWORD         dwFlags
            if (!rc) log("%%UpdateLayeredWindow returns unexpected result ", prompt::gui, rc);
            sync = true;
        }
    };

    struct manager
    {
        using wins = std::vector<surface>;

        struct gcfg
        {
            IDWriteFactory2*       pDWriteFactory{};
            IDWriteFontFallback*   systemFontFallback{};
            IDWriteFontCollection* systemFontCollection{};

            //IDWriteTextFormat* pTextFormat[4]{};
            void reset()
            {
                systemFontCollection->Release();
                systemFontFallback->Release();
                pDWriteFactory->Release();
            }
        };

        enum bttn
        {
            left   = 1 << 0,
            right  = 1 << 1,
            middle = 1 << 2,
        };

        gcfg config; // manager: DWrite's references.
        bool isfine; // manager: All is ok.
        wins layers; // manager: ARGB layers.
        wide locale; // manager: Locale.
        wide family; // manager: Base font family name.
        fp32 fontsz; // manager: Base font size.

        manager(text font_name_utf8, twod cellsz)
            : isfine{ faux },
              locale(LOCALE_NAME_MAX_LENGTH, '\0'),
              family{ utf::to_utf(font_name_utf8) },
              fontsz{ cellsz.y * 16.f / 22.f } //todo temp
        {
            set_dpi_awareness();
            ok2(::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&config.pDWriteFactory)));

            config.pDWriteFactory->GetSystemFontFallback(&config.systemFontFallback);
            config.pDWriteFactory->GetSystemFontCollection(&config.systemFontCollection, TRUE);

            if (!::GetUserDefaultLocaleName(locale.data(), (si32)locale.size())) // Return locale length or 0.
            {
                locale = L"en-US";
                log("%%Using default locale 'en-US'.", prompt::gui);
            }
            isfine = true;
        }
        ~manager()
        {
            for (auto& w : layers) w.reset();
            config.reset();
        }
        auto& operator [] (si32 layer) { return layers[layer]; }
        explicit operator bool () const { return isfine; }

        void set_dpi_awareness()
        {
            auto proc = (LONG(_stdcall *)(si32))::GetProcAddress(::GetModuleHandleA("user32.dll"), "SetProcessDpiAwarenessInternal");
            if (proc)
            {
                auto hr = proc(2/*PROCESS_PER_MONITOR_DPI_AWARE*/);
                if (hr != S_OK || hr != E_ACCESSDENIED) log("%%Set DPI awareness failed %hr% %ec%", prompt::gui, utf::to_hex(hr), ::GetLastError());
            }
        }
        auto set_dpi(auto new_dpi)
        {
            //for (auto& w : layers) w.set_dpi(new_dpi);
            log("%%DPI changed to %dpi%", prompt::gui, new_dpi);
        }
        auto moveby(twod delta)
        {
            for (auto& w : layers) w.area.coor += delta;
        }
        template<bool JustMove = faux>
        void present()
        {
            if constexpr (JustMove)
            {
                auto lock = ::BeginDeferWindowPos((si32)layers.size());
                for (auto& w : layers)
                {
                    lock = ::DeferWindowPos(lock, w.hWnd, 0, w.area.coor.x, w.area.coor.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
                    if (!lock) { log("%%DeferWindowPos returns unexpected result: %ec%", prompt::gui, ::GetLastError()); }
                    w.prev.coor = w.area.coor;
                }
                ::EndDeferWindowPos(lock);
            }
            else for (auto& w : layers) w.present(); // 3.000 ms
        }
        void dispatch()
        {
            auto msg = MSG{};
            while (::GetMessageW(&msg, 0, 0, 0) > 0)
            {
                ::DispatchMessageW(&msg);
            }
        }
        void shown_event(bool shown, arch reason)
        {
            log(shown ? "shown" : "hidden", " ", reason == SW_OTHERUNZOOM   ? "The window is being uncovered because a maximize window was restored or minimized."
                                               : reason == SW_OTHERZOOM     ? "The window is being covered by another window that has been maximized."
                                               : reason == SW_PARENTCLOSING ? "The window's owner window is being minimized."
                                               : reason == SW_PARENTOPENING ? "The window's owner window is being restored."
                                                                            : "unknown reason");
        }
        void show(si32 win_state)
        {
            if (win_state == 0 || win_state == 2) //todo fullscreen mode (=2). 0 - normal, 1 - minimized, 2 - fullscreen
            {
                auto mode = SW_SHOWNORMAL;
                for (auto& w : layers) { ::ShowWindow(w.hWnd, mode); }
            }
        }
        void mouse_capture()
        {
            if (!layers.empty()) ::SetCapture(layers.front().hWnd);
        }
        void mouse_release()
        {
            ::ReleaseCapture();
        }
        void close()
        {
            if (!layers.empty()) ::SendMessageW(layers.front().hWnd, WM_CLOSE, NULL, NULL);
        }
        void activate()
        {
            log("activated");
            if (!layers.empty()) ::SetActiveWindow(layers.front().hWnd);
        }
        void state_event(bool activated, bool minimized)
        {
            log(activated ? "activated" : "deactivated", " ", minimized ? "minimized" : "restored");
        }

        virtual void update() = 0;
        virtual void mouse_shift(twod coord) = 0;
        virtual void focus_event(bool state) = 0;
        virtual void mouse_press(si32 index, bool pressed) = 0;
        virtual void mouse_wheel(si32 delta, bool hzwheel) = 0;
        virtual void keybd_press(arch vkey, arch lParam) = 0;

        auto add(manager* host_ptr = nullptr)
        {
            auto window_proc = [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
            {
                //log("\tmsW=", utf::to_hex(msg), " wP=", utf::to_hex(wParam), " lP=", utf::to_hex(lParam), " hwnd=", utf::to_hex(hWnd));
                //auto layer = (si32)::GetWindowLongPtrW(hWnd, sizeof(LONG_PTR));
                auto w = (manager*)::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
                if (!w) return ::DefWindowProcW(hWnd, msg, wParam, lParam);
                auto stat = LRESULT{};
                static auto hi = [](auto n){ return (si32)(si16)((n >> 16) & 0xffff); };
                static auto lo = [](auto n){ return (si32)(si16)((n >> 0 ) & 0xffff); };
                static auto hover_win = testy<HWND>{};
                static auto hover_rec = TRACKMOUSEEVENT{ .cbSize = sizeof(TRACKMOUSEEVENT), .dwFlags = TME_LEAVE, .dwHoverTime = HOVER_DEFAULT };
                static auto h = 0;
                static auto f = 0;
                switch (msg)
                {
                    case WM_MOUSEMOVE:
                        if (hover_win(hWnd)) ::TrackMouseEvent((++h, hover_rec.hwndTrack = hWnd, &hover_rec));
                        if (auto r = RECT{}; ::GetWindowRect(hWnd, &r)) w->mouse_shift({ r.left + lo(lParam), r.top + hi(lParam) });
                        break;
                    case WM_MOUSELEAVE:  if (!--h) w->mouse_shift(dot_mx), hover_win = {}; break;
                    case WM_ACTIVATEAPP: if (!(wParam ? f++ : --f)) w->focus_event(f); break; // Focus between apps.
                    case WM_ACTIVATE:      w->state_event(!!lo(wParam), !!hi(wParam)); break; // Window focus within the app.
                    case WM_MOUSEACTIVATE: w->activate(); stat = MA_NOACTIVATE;        break; // Suppress window activation with a mouse click.
                    case WM_LBUTTONDOWN:   w->mouse_press(bttn::left,   true);         break;
                    case WM_MBUTTONDOWN:   w->mouse_press(bttn::middle, true);         break;
                    case WM_RBUTTONDOWN:   w->mouse_press(bttn::right,  true);         break;
                    case WM_LBUTTONUP:     w->mouse_press(bttn::left,   faux);         break;
                    case WM_MBUTTONUP:     w->mouse_press(bttn::middle, faux);         break;
                    case WM_RBUTTONUP:     w->mouse_press(bttn::right,  faux);         break;
                    case WM_MOUSEWHEEL:    w->mouse_wheel(hi(wParam), faux);           break;
                    case WM_MOUSEHWHEEL:   w->mouse_wheel(hi(wParam), true);           break;
                    case WM_SHOWWINDOW:    w->shown_event(!!wParam, lParam);           break; //todo revise
                    //case WM_GETMINMAXINFO: w->maximize(wParam, lParam);              break; // The system is about to maximize the window.
                    //case WM_SYSCOMMAND:  w->sys_command(wParam, lParam);             break; //todo taskbar ctx menu to change the size and position
                    case WM_KEYDOWN:
                    case WM_KEYUP:
                    case WM_SYSKEYDOWN:  // WM_CHAR/WM_SYSCHAR and WM_DEADCHAR/WM_SYSDEADCHAR are derived messages after translation.
                    case WM_SYSKEYUP:      w->keybd_press(wParam, lParam);             break;
                    case WM_DPICHANGED:    w->set_dpi(lo(wParam));                     break;
                    case WM_DESTROY:       ::PostQuitMessage(0);                       break;
                    //dx3d specific
                    //case WM_PAINT:   /*w->check_dx3d_state();*/ stat = ::DefWindowProcW(hWnd, msg, wParam, lParam); break;
                    default:                                    stat = ::DefWindowProcW(hWnd, msg, wParam, lParam); break;
                }
                w->update();
                return stat;
            };
            static auto wc_defwin = WNDCLASSW{ .lpfnWndProc = ::DefWindowProcW, .lpszClassName = L"vtm_decor" };
            static auto wc_window = WNDCLASSW{ .lpfnWndProc = window_proc, /*.cbWndExtra = 2 * sizeof(LONG_PTR),*/ .hCursor = ::LoadCursorW(NULL, (LPCWSTR)IDC_ARROW), .lpszClassName = L"vtm" };
            static auto reg = ::RegisterClassW(&wc_defwin) && ::RegisterClassW(&wc_window);
            if (!reg)
            {
                isfine = faux;
                log("%%window class registration error: %ec%", prompt::gui, ::GetLastError());
            }
            auto& wc = host_ptr ? wc_window : wc_defwin;
            auto owner = layers.empty() ? HWND{} : layers.front().hWnd;
            auto hWnd = ::CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | (wc.hCursor ? 0 : WS_EX_TRANSPARENT),
                                          wc.lpszClassName, owner ? nullptr : wc.lpszClassName, // Title.
                                          WS_POPUP /*todo | owner ? WS_SYSMENU : 0  taskbar ctx menu*/, 0, 0, 0, 0, owner, 0, 0, 0);
            auto layer = (si32)layers.size();
            if (!hWnd)
            {
                isfine = faux;
                log("%%Window creation error: %ec%", prompt::gui, ::GetLastError());
            }
            else if (host_ptr)
            {
                ::SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)host_ptr);
                //::SetWindowLongPtrW(hWnd, 0, (LONG_PTR)host_ptr);
                //::SetWindowLongPtrW(hWnd, sizeof(LONG_PTR), (LONG_PTR)layer);
            }
            layers.emplace_back(hWnd);
            return layer;
        }
    };

    struct window : manager
    {
        using gray = netxs::raster<std::vector<byte>, rect>;
        using shad = netxs::misc::shadow<gray>;
        using grip = netxs::misc::szgrips;

        struct task
        {
            static constexpr auto _counter = 1 + __COUNTER__;
            static constexpr auto moved  = 1 << (__COUNTER__ - _counter);
            static constexpr auto sized  = 1 << (__COUNTER__ - _counter);
            static constexpr auto grips  = 1 << (__COUNTER__ - _counter);
            static constexpr auto hover  = 1 << (__COUNTER__ - _counter);
            static constexpr auto inner  = 1 << (__COUNTER__ - _counter);
            static constexpr auto header = 1 << (__COUNTER__ - _counter);
            static constexpr auto footer = 1 << (__COUNTER__ - _counter);
            static constexpr auto all = -1;
        };

        std::unordered_map<ui64, alpha_mask> glyph_cache;
        ui::face grid_data;

        twod gridsz; // window: Grid size in cells.
        twod cellsz; // window: Cell size in pixels.
        twod gripsz; // window: Resizing grips size in pixels.
        dent border; // window: Border around window for resizing grips (dent in pixels).
        shad shadow; // window: Shadow generator.
        grip szgrip; // window: Resizing grips UI-control.
        twod mcoord; // window: Mouse cursor coord.
        si32 mbttns; // window: Mouse button state.
        si32 reload; // window: Changelog for update.
        si32 client; // window: Surface index for Client.
        si32 grip_l; // window: Surface index for Left resizing grip.
        si32 grip_r; // window: Surface index for Right resizing grip.
        si32 grip_t; // window: Surface index for Top resizing grip.
        si32 grip_b; // window: Surface index for Bottom resizing grip.
        si32 header; // window: Surface index for Header.
        si32 footer; // window: Surface index for Footer.
        wide toWide; // window: UTF-16 buffer.
        bool drop_shadow{ true };

        static constexpr auto shadow_dent = dent{ 1,1,1,1 } * 3;

        window(rect win_coor_px_size_cell, text font, twod cell_size = { 10, 20 }, si32 win_mode = 0, twod grip_cell = { 2, 1 })
            : manager{ font, cell_size },
              gridsz{ std::max(dot_11, win_coor_px_size_cell.size) },
              cellsz{ cell_size },
              gripsz{ grip_cell * cell_size },
              border{ gripsz.x, gripsz.x, gripsz.y, gripsz.y },
              shadow{ 0.44f/*bias*/, 116.5f/*alfa*/, gripsz.x, dot_00, dot_11, cell::shaders::full },
              mbttns{},
              reload{ task::all },
              client{ add(this) },
              grip_l{ add(this) },
              grip_r{ add(this) },
              grip_t{ add(this) },
              grip_b{ add(this) },
              header{ add() },
              footer{ add() }
        {
            if (!*this) return;
            layers[client].area = { win_coor_px_size_cell.coor, gridsz * cellsz };
            recalc_layout();
            //todo temp
            grid_data.size(layers[client].area.size / cellsz);
            grid_data.cup(dot_00);
            grid_data.output(canvas_page);

            update();
            manager::show(win_mode);
        }
        void recalc_layout()
        {
            auto base_rect = layers[client].area;
            layers[grip_l].area = base_rect + dent{ gripsz.x, -base_rect.size.x, gripsz.y, gripsz.y };
            layers[grip_r].area = base_rect + dent{ -base_rect.size.x, gripsz.x, gripsz.y, gripsz.y };
            layers[grip_t].area = base_rect + dent{ 0, 0, gripsz.y, -base_rect.size.y };
            layers[grip_b].area = base_rect + dent{ 0, 0, -base_rect.size.y, gripsz.y };
            auto header_height = cellsz.y * ((header_para.size().x + gridsz.x - 1) / gridsz.x);
            auto footer_height = cellsz.y * ((footer_para.size().x + gridsz.x - 1) / gridsz.x);
            layers[header].area = base_rect + dent{ 0, 0, header_height, -base_rect.size.y } + shadow_dent;
            layers[footer].area = base_rect + dent{ 0, 0, -base_rect.size.y, footer_height } + shadow_dent;
            layers[header].area.coor.y -= shadow_dent.b;
            //layers[footer].area.coor.y += shadow_dent.t;
        }
        auto move_window(twod coor_delta)
        {
            manager::moveby(coor_delta);
            reload |= task::moved;
        }
        auto size_window(twod size_delta)
        {
            layers[client].area.size += size_delta;
            recalc_layout();
            reload |= task::sized;
            //todo temp
            grid_data.size(layers[client].area.size / cellsz);
            grid_data.cup(dot_00);
            grid_data.output(canvas_page);
        }
        auto resize_window(twod size_delta)
        {
            auto old_client = layers[client].area;
            auto new_gridsz = std::max(dot_11, (old_client.size + size_delta) / cellsz);
            size_delta = dot_00;
            if (gridsz != new_gridsz)
            {
                gridsz = new_gridsz;
                size_delta = gridsz * cellsz - old_client.size;
                size_window(size_delta);
            }
            return size_delta;
        }
        auto warp_window(dent warp_delta)
        {
            auto old_client = layers[client].area;
            auto new_client = old_client + warp_delta;
            auto new_gridsz = std::max(dot_11, new_client.size / cellsz);
            if (gridsz != new_gridsz)
            {
                gridsz = new_gridsz;
                auto size_delta = gridsz * cellsz - old_client.size;
                auto coor_delta = new_client.coor - old_client.coor;
                size_window(size_delta);
                move_window(coor_delta);
            }
            return layers[client].area - old_client;
        }
        void generatee_glyph_mak(alpha_mask& glyph_mask, cell const& c)
        {
            glyph_mask.type = alpha_mask::plain;
            if (c.wdt() == 0) return;
            auto format = style::normal;
            if (c.itc()) format |= style::italic;
            if (c.bld()) format |= style::bold;
            auto code_iter = utf::cpit{ c.txt() };
            static auto code_buff = std::vector<ui32>{};
            static auto glyph_index = std::vector<ui16>{};
            code_buff.resize(0);
            while (code_iter) code_buff.push_back(code_iter.next().cdpoint);
            if (code_buff.empty()) return;

            //auto maxRangeCount = ui32{ 100 };
            //auto actualRangeCount = ui32{};
            //auto unicodeRanges = std::vector<DWRITE_UNICODE_RANGE>(maxRangeCount);
            //fontFace->GetUnicodeRanges(maxRangeCount, unicodeRanges.data(), &actualRangeCount);

            struct my_IDWriteTextAnalysisSource : IDWriteTextAnalysisSource
            {
                struct myNS : IDWriteNumberSubstitution
                {
                    ui32 rcount{};
                    IFACEMETHOD_(unsigned long, AddRef)() { return InterlockedIncrement(&rcount); }
                    IFACEMETHOD_(unsigned long, Release)()
                    {
                        auto new_refs = InterlockedDecrement(&rcount);
                        if (new_refs == 0) delete this;
                        return new_refs;
                    }
                    IFACEMETHOD(QueryInterface)(IID const& riid, void** ppvObject)
                    {
                             if (__uuidof(IDWriteNumberSubstitution) == riid) *ppvObject = this;
                        else if (__uuidof(IUnknown)                  == riid) *ppvObject = this;
                        else                                                { *ppvObject = NULL; return E_FAIL; }
                        return S_OK;
                    }
                } myNS;
                wide& locale;
                wide& letter;
                ui32 rcount{};
                my_IDWriteTextAnalysisSource(wide& locale, wide& letter)
                    : locale{ locale },
                      letter{ letter }
                { }

                IFACEMETHOD_(unsigned long, AddRef)() { return InterlockedIncrement(&rcount); }
                IFACEMETHOD_(unsigned long, Release)()
                {
                    //not used
                    auto new_refs = InterlockedDecrement(&rcount);
                    if (new_refs == 0) delete this;
                    return new_refs;
                }
                IFACEMETHOD(QueryInterface)(IID const& riid, void** ppvObject)
                {
                         if (__uuidof(IDWriteTextAnalysisSource) == riid) *ppvObject = this;
                    else if (__uuidof(IUnknown)                  == riid) *ppvObject = this;
                    else                                                { *ppvObject = NULL; return E_FAIL; }
                    return S_OK;
                }
                IFACEMETHOD(GetTextAtPosition)(UINT32 textPosition, WCHAR const** textString, UINT32* textLength)
                {
                    if (textPosition < letter.size())
                    {
                        *textString = letter.data() + textPosition;
                        *textLength = (ui32)std::max(0, (si32)letter.size() - (si32)textPosition);
                    }
                    else
                    {
                        *textString = nullptr;
                        *textLength = 0;
                    }
                    return S_OK;
                }
                IFACEMETHOD(GetTextBeforePosition)(UINT32 textPosition, WCHAR const** textString, UINT32* textLength)
                {
                    //not used
                    if (textPosition > 0 && textPosition <= letter.size())
                    {
                        *textString = letter.data();
                        *textLength = textPosition;
                    }
                    else
                    {
                        *textString = nullptr;
                        *textLength = 0;
                    }
                    return S_OK;
                }
                IFACEMETHOD_(DWRITE_READING_DIRECTION, GetParagraphReadingDirection)()
                {
                    //not used
                    return DWRITE_READING_DIRECTION_LEFT_TO_RIGHT;
                }
                IFACEMETHOD(GetLocaleName)(UINT32 textPosition, UINT32* textLength, WCHAR const** localeName)
                {
                    *textLength = (ui32)std::max(0, (si32)letter.size() - (si32)textPosition);
                    *localeName = locale.data();
                    return S_OK;
                }
                IFACEMETHOD(GetNumberSubstitution)(UINT32 textPosition, UINT32* textLength, IDWriteNumberSubstitution** numberSubstitution)
                {
                    //not used
                    *textLength = (ui32)std::max(0, (si32)letter.size() - (si32)textPosition);
                    *numberSubstitution = &myNS;
                    myNS.AddRef();
                    return S_OK;
                    //return E_NOTIMPL;
                }
            };

            auto scale = fp32{};
            auto mappedLength = ui32{};
            auto mappedFont = (IDWriteFont2*)nullptr;
            toWide.clear();
            utf::to_utf(c.txt(), toWide);
            auto analysisSource = my_IDWriteTextAnalysisSource{ manager::locale, toWide };
            auto hr = config.systemFontFallback->MapCharacters(
                &analysisSource,                    // IDWriteTextAnalysisSource* analysisSource,
                0,                                  // UINT32 textPosition,
                toWide.size(),                      // UINT32 textLength,
                config.systemFontCollection,        // _In_opt_ IDWriteFontCollection* baseFontCollection,
                manager::family.data(),             // _In_opt_z_ wchar_t const* baseFamilyName,
                c.bld() ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,          // DWRITE_FONT_WEIGHT baseWeight,
                c.itc() ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,          // DWRITE_FONT_STYLE baseStyle,
                DWRITE_FONT_STRETCH_NORMAL,         // DWRITE_FONT_STRETCH baseStretch,
                &mappedLength,                      // _Out_range_(0, textLength) UINT32* mappedLength,
                (IDWriteFont**)&mappedFont,         // _COM_Outptr_result_maybenull_ IDWriteFont** mappedFont,
                &scale);                            // _Out_ FLOAT* scale
            if (!mappedFont)
            {
                log("no font for ", c.txt());
                return;
            }
            auto exists = BOOL{};
            mappedFont->HasCharacter(code_buff.front(), &exists);
            if (!exists)
            {
                auto names = (IDWriteLocalizedStrings*)nullptr;
                auto fontFamily = (IDWriteFontFamily*)nullptr;
                mappedFont->GetFontFamily(&fontFamily);
                fontFamily->GetFamilyNames(&names);
                auto buff = wide(100, 0);
                names->GetString(0, buff.data(), buff.size());
                log("no glyph '", c.txt(), "' in font: ", utf::to_utf(buff.data()));
                names->Release();
                fontFamily->Release();
            }

            auto fontFace = (IDWriteFontFace2*)nullptr;
            hr = mappedFont->CreateFontFace((IDWriteFontFace**)&fontFace);

            glyph_index.resize(code_buff.size());
            hr = fontFace->GetGlyphIndices(code_buff.data(), (ui32)code_buff.size(), glyph_index.data());

            auto glyphRun = DWRITE_GLYPH_RUN{ .fontFace = fontFace,
                                              .fontEmSize = fontsz,
                                              .glyphCount = (ui32)glyph_index.size(),
                                              .glyphIndices = glyph_index.data() };

            auto para_layers = (IDWriteColorGlyphRunEnumerator*)nullptr;
            auto measuringMode = DWRITE_MEASURING_MODE_NATURAL;
            hr = config.pDWriteFactory->TranslateColorGlyphRun(0, 0, &glyphRun, nullptr, measuringMode, nullptr, 0, &para_layers);
            auto color = !!para_layers;
            auto rendering_mode = color ? DWRITE_RENDERING_MODE_ALIASED : DWRITE_RENDERING_MODE_GDI_NATURAL;
            auto grid_fit_mode = DWRITE_GRID_FIT_MODE_DEFAULT;
            auto aa_mode = DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE; // DWRITE_TEXT_ANTIALIAS_MODE_CLEARTYPE
            auto gen_texture = [&](auto& glyphRun)
            {
                auto glyphRunAnalysis = (IDWriteGlyphRunAnalysis*)nullptr;
                config.pDWriteFactory->CreateGlyphRunAnalysis(&glyphRun,                     // glyphRun,
                                                            nullptr,                       // transform,
                                                            rendering_mode,                // renderingMode,
                                                            measuringMode,                 // measuringMode,
                                                            grid_fit_mode,                 // gridFitMode,
                                                            aa_mode,                       // antialiasMode,
                                                            0,                             // baselineOriginX,
                                                            0,                             // baselineOriginY,
                                                            &glyphRunAnalysis);            // glyphRunAnalysis
                auto r = RECT{};
                hr = glyphRunAnalysis->GetAlphaTextureBounds(DWRITE_TEXTURE_ALIASED_1x1, &r);
                glyph_mask.area.size = { r.right - r.left, r.bottom - r.top };
                glyph_mask.bits.resize(glyph_mask.area.size.x * glyph_mask.area.size.y);
                hr = glyphRunAnalysis->CreateAlphaTexture(DWRITE_TEXTURE_ALIASED_1x1, &r, glyph_mask.bits.data(), (ui32)glyph_mask.bits.size());
                //todo apply glyph metrics
                glyphRunAnalysis->Release();
            };
            if (para_layers)
            {
                auto subrun = (DWRITE_COLOR_GLYPH_RUN const*)nullptr;
                auto next = BOOL{ true };
                while (para_layers->MoveNext(&next), next)
                {
                    hr = para_layers->GetCurrentRun(&subrun);
                    auto& s = *subrun;
                    auto color = argb{ s.runColor.r, s.runColor.g, s.runColor.b, s.runColor.a }; // s.runColor.a could be nan != 0.
                    if (hr == S_OK && color.chan.a)
                    {
                        if (color.token == 0) color.token = 0xFF'01'01'01; // Don't allow pure black fg for color glyphs.
                        //auto token = s.paletteIndex == -1 ? fgc.token : color.token;
                        gen_texture(subrun->glyphRun);
                        break;
                    }
                }
                para_layers->Release();
            }
            else if (hr == DWRITE_E_NOCOLOR)
            {
                gen_texture(glyphRun);
            }
            fontFace->Release();
            mappedFont->Release();
        }
        void draw_cell(cell const& c, auto iter, si32 region_size_x)
        {
            auto w = c.wdt();
            if (w == 0) return;
            auto token = c.tkn() & ~3;
            if (c.itc()) token |= style::italic;
            if (c.bld()) token |= style::bold;
            auto& glyph_mask = glyph_cache[token];
            if (glyph_mask.type == alpha_mask::undef) generatee_glyph_mak(glyph_mask, c);
            //todo underline/strike etc
            if (!glyph_mask.area) return;

            auto line_y = region_size_x * (glyph_mask.area.size.y - 1);
            auto stride = region_size_x - glyph_mask.area.size.x;
            auto step_x = glyph_mask.area.size.x;
            auto step_y = glyph_mask.area.size.x + line_y;
            auto dst = iter + glyph_mask.area.coor.x + glyph_mask.area.coor.y * region_size_x;
            if (w == 1)
            {
                if (glyph_mask.type == alpha_mask::color)
                {
                    auto src = (argb*)glyph_mask.bits.data();
                    auto blendpma = [&](auto& d){ d.blendpma(*src++); };
                    netxs::inrect(dst, step_x, step_y, stride, blendpma);
                }
                else
                {
                    auto fgc = c.fgc();
                    auto src = (byte*)glyph_mask.bits.data();
                    auto colorize = [&](argb& dst){ dst.mix(fgc, *src++); };
                    netxs::inrect(dst, step_x, step_y, stride, colorize);
                }
            }
            else
            {
                step_x /= 2;
                auto ddx = glyph_mask.area.size.x & 1;
                if (w == 2)
                {
                    step_y -= step_x;
                    stride += step_x;
                    step_x += ddx;
                    if (glyph_mask.type == alpha_mask::color)
                    {
                        auto src = (argb*)glyph_mask.bits.data();
                        auto blendpma = [&](auto& dst){ dst.blendpma(*src++); };
                        auto online = [&]{ src += glyph_mask.area.size.x / 2; };
                        netxs::inrect(dst, step_x, step_y, stride, blendpma, online);
                    }
                    else
                    {
                        auto fgc = c.fgc();
                        auto src = (byte*)glyph_mask.bits.data();
                        auto colorize = [&](argb& dst){ dst.mix(fgc, *src++); };
                        auto online = [&]{ src += glyph_mask.area.size.x / 2; };
                        netxs::inrect(dst, step_x, step_y, stride, colorize, online);
                    }
                }
                else if (w == 3)
                {
                    ddx += step_x;
                    step_y -= ddx;
                    stride += ddx;
                    if (glyph_mask.type == alpha_mask::color)
                    {
                        auto src = (argb*)glyph_mask.bits.data();
                        src += ddx;
                        auto blendpma = [&](auto& dst){ dst.blendpma(*src++); };
                        auto online = [&]{ src += ddx; };
                        netxs::inrect(dst, step_x, step_y, stride, blendpma, online);
                    }
                    else
                    {
                        auto fgc = c.fgc();
                        auto src = (byte*)glyph_mask.bits.data();
                        src += ddx;
                        auto colorize = [&](argb& dst){ dst.mix(fgc, *src++); };
                        auto online = [&]{ src += ddx; };
                        netxs::inrect(dst, step_x, step_y, stride, colorize, online);
                    }
                }
            }
        }
        void draw_grid()
        {
            auto canvas = layers[client].canvas(); // 0.500 ms
            auto region = canvas.area();
            auto r  = rect{ .size = cellsz };
            auto lt = dent{ 1, 0, 1, 0 };
            auto rb = dent{ 0, 1, 0, 1 };
            canvas.step(-region.coor);
            auto rtc = argb{ tint::pureblue  };//.alpha(0.5f);
            auto ltc = argb{ tint::pureblack };
            auto rbc = argb{ tint::pureblack };
            auto lbc = argb{ tint::pureblue  };//.alpha(0.5f);

            auto line_y = region.size.x * (cellsz.y - 1);
            auto offset = 0;
            auto stride = region.size.x - cellsz.x;
            auto step_x = cellsz.x;
            auto step_y = cellsz.x + line_y;
            for (r.coor.y = 0; r.coor.y < region.size.y; r.coor.y += cellsz.y)
            {
                auto fy = (fp32)r.coor.y / (region.size.y - 1);
                auto lc = argb::transit(ltc, lbc, fy);
                auto rc = argb::transit(rtc, rbc, fy);
                for (r.coor.x = 0; r.coor.x < region.size.x; r.coor.x += cellsz.x)
                {
                    auto fx = (fp32)r.coor.x / (region.size.x - 1);
                    auto p = argb::transit(lc, rc, fx);
                    netxs::inrect(canvas.begin() + offset, step_x, step_y, stride, cell::shaders::full(p));
                    offset += step_x;
                }
                offset += line_y;
            }

            canvas.step(region.coor);
            auto& layer = layers[client];
            auto coor = dot_00;
            offset = region.size.x * (coor.y * cellsz.y) + coor.x;
            for (auto& c : grid_data)
            {
                draw_cell(c, canvas.begin() + offset, region.size.x);
                offset += step_x;
                coor.x += cellsz.x;
                if (coor.x >= region.size.x)
                {
                    coor.x = 0;
                    coor.y += cellsz.y;
                    offset += line_y;
                    if (coor.y >= region.size.y) break;
                }
            }
        }
        bool hit_grips()
        {
            auto inner_rect = layers[client].area;
            auto outer_rect = layers[client].area + border;
            auto hit = !szgrip.zoomon && (szgrip.seized || (outer_rect.hittest(mcoord) && !inner_rect.hittest(mcoord)));
            return hit;
        }
        void fill_grips(rect area, auto fx)
        {
            for (auto g : { grip_l, grip_r, grip_t, grip_b })
            {
                auto& layer = layers[g];
                if (auto r = layer.area.trim(area))
                {
                    auto canvas = layer.canvas();
                    fx(canvas, r);
                }
            }
        }
        void draw_grips()
        {
            static auto trans = 0x01'00'00'00;
            static auto shade = 0x5F'3f'3f'3f;
            static auto black = 0x3F'00'00'00;
            auto inner_rect = layers[client].area;
            auto outer_rect = layers[client].area + border;
            fill_grips(outer_rect, [](auto& canvas, auto r){ netxs::misc::fill(canvas, r, cell::shaders::full(trans)); });
            if (hit_grips())
            {
                auto s = szgrip.sector;
                auto [side_x, side_y] = szgrip.layout(outer_rect);
                auto dent_x = dent{ s.x < 0, s.x > 0, s.y > 0, s.y < 0 };
                auto dent_y = dent{ s.x > 0, s.x < 0, 1, 1 };
                fill_grips(side_x, [&](auto& canvas, auto r)
                {
                    netxs::misc::fill(canvas, r, cell::shaders::full(shade));
                    netxs::misc::cage(canvas, side_x, dent_x, cell::shaders::full(black)); // 1-px dark contour around.
                });
                fill_grips(side_y, [&](auto& canvas, auto r)
                {
                    netxs::misc::fill(canvas, r, cell::shaders::full(shade));
                    netxs::misc::cage(canvas, side_y, dent_y, cell::shaders::full(black)); // 1-px dark contour around.
                });
            }
            if (drop_shadow) fill_grips(outer_rect, [&](auto& canvas, auto r)
            {
                shadow.render(canvas, r, inner_rect, cell::shaders::alpha);
            });
        }
        void draw_title(si32 index, twod align, wiew utf8) //todo just output ui::core
        {
            auto& layer = layers[index];
            auto canvas = layer.canvas(true);
            auto r = canvas.area().moveto(dot_00).rotate(align) - shadow_dent;
            //layer.textout(r, dot_00, 0xFF'ff'ff'ff, style::normal, utf8);
            netxs::misc::contour(canvas); // 1ms
        }
        void draw_header() { draw_title(header, { 1, -1 }, header_text); }
        void draw_footer() { draw_title(footer, { -1, 1 }, footer_text); }
        void update()
        {
            if (!reload) return;
            auto what = reload;
            reload = {};
                 if (what == task::moved) manager::present<true>();
            else if (what)
            {
                if (what & (task::sized | task::inner))   draw_grid();
                if (what & (task::sized | task::hover | task::grips)) draw_grips(); // 0.150 ms
                if (what & (task::sized | task::header)) draw_header();
                if (what & (task::sized | task::footer)) draw_footer();
                //if (layers[client].area.hittest(mcoord))
                //{
                //    auto cursor = rect{ mcoord - (mcoord - layers[client].area.coor) % cellsz, cellsz };
                //    netxs::onrect(layers[client].canvas(), cursor, cell::shaders::full(0x7F'00'3f'00));
                //}
                manager::present();
            }
        }
        auto& kbs()
        {
            static auto state_kb = 0;
            return state_kb;
        }
        auto keybd_state()
        {
            //todo unify
            auto state = hids::LShift   * !!::GetAsyncKeyState(VK_LSHIFT)
                       | hids::RShift   * !!::GetAsyncKeyState(VK_RSHIFT)
                       | hids::LWin     * !!::GetAsyncKeyState(VK_LWIN)
                       | hids::RWin     * !!::GetAsyncKeyState(VK_RWIN)
                       | hids::LAlt     * !!::GetAsyncKeyState(VK_LMENU)
                       | hids::RAlt     * !!::GetAsyncKeyState(VK_RMENU)
                       | hids::LCtrl    * !!::GetAsyncKeyState(VK_LCONTROL)
                       | hids::RCtrl    * !!::GetAsyncKeyState(VK_RCONTROL)
                       | hids::ScrlLock * !!::GetKeyState(VK_SCROLL)
                       | hids::NumLock  * !!::GetKeyState(VK_NUMLOCK)
                       | hids::CapsLock * !!::GetKeyState(VK_CAPITAL);
            return state;
        }
        void mouse_wheel(si32 delta, bool /*hz*/)
        {
            auto wheeldt = delta / 120;
            auto kb = kbs();
            //     if (kb & (hids::LCtrl | hids::LAlt)) netxs::_k2 += wheeldt > 0 ? 1 : -1; // LCtrl + Alt t +Wheel.
            //else if (kb & hids::LCtrl)                netxs::_k0 += wheeldt > 0 ? 1 : -1; // LCtrl+Wheel.
            //else if (kb & hids::anyAlt)               netxs::_k1 += wheeldt > 0 ? 1 : -1; // Alt+Wheel.
            //else if (kb & hids::RCtrl)                netxs::_k3 += wheeldt > 0 ? 1 : -1; // RCtrl+Wheel.
            //shadow = build_shadow_corner(cellsz.x);
            //reload |= task::sized;
            //netxs::_k0 += wheeldt > 0 ? 1 : -1;
            //log("wheel ", wheeldt, " k0= ", _k0, " k1= ", _k1, " k2= ", _k2, " k3= ", _k3, " keybd ", utf::to_bin(kb));

            if ((kb & hids::anyCtrl) && !(kb & hids::ScrlLock))
            {
                if (!szgrip.zoomon)
                {
                    szgrip.zoomdt = {};
                    szgrip.zoomon = true;
                    szgrip.zoomsz = layers[client].area;
                    szgrip.zoomat = mcoord;
                    mouse_capture();
                }
            }
            else if (szgrip.zoomon)
            {
                szgrip.zoomon = faux;
                mouse_release();
            }
            if (szgrip.zoomon)
            {
                auto warp = dent{ gripsz.x, gripsz.x, gripsz.y, gripsz.y };
                auto step = szgrip.zoomdt + warp * wheeldt;
                auto next = szgrip.zoomsz + step;
                next.size = std::max(dot_00, next.size);
                ///auto viewport = ...get max win size (multimon)
                //next.trimby(viewport);
                if (warp_window(next - layers[client].area)) szgrip.zoomdt = step;
            }
        }
        void mouse_shift(twod coord)
        {
            auto kb = kbs();// keybd_state();
            auto inner_rect = layers[client].area;
            if (hit_grips() || szgrip.seized)
            {
                if (mbttns & bttn::left)
                {
                    if (!szgrip.seized) // drag start
                    {
                        szgrip.grab(inner_rect, mcoord, border, cellsz);
                    }
                    auto zoom = kb & hids::anyCtrl;
                    auto [preview_area, size_delta] = szgrip.drag(inner_rect, coord, border, zoom, cellsz);
                    if (auto dxdy = resize_window(size_delta))
                    {
                        if (auto move_delta = szgrip.move(dxdy, zoom))
                        {
                            move_window(move_delta);
                        }
                    }
                }
                else if (szgrip.seized) // drag stop
                {
                    szgrip.drop();
                    reload |= task::grips;
                }
            }
            if (szgrip.zoomon && !(kb & hids::anyCtrl))
            {
                szgrip.zoomon = faux;
                mouse_release();
            }
            if (szgrip.calc(inner_rect, coord, border, dent{}, cellsz))
            {
                reload |= task::grips;
            }
            if (!szgrip.seized && mbttns & bttn::left)
            {
                if (auto dxdy = coord - mcoord)
                {
                    manager::moveby(dxdy);
                    reload |= task::moved;
                }
            }
            mcoord = coord;
            if (!mbttns)
            {
                static auto s = testy{ faux };
                reload |= s(hit_grips()) ? task::grips | task::inner
                                     : s ? task::grips : task::inner;
            }
        }
        void mouse_press(si32 button, bool pressed)
        {
            if (pressed && !mbttns) mouse_capture();
            pressed ? mbttns |= button
                    : mbttns &= ~button;
            if (!mbttns) mouse_release();
            if (!pressed & (button == bttn::right)) manager::close();
        }
        void keybd_press(arch vkey, arch lParam)
        {
            union key_state
            {
                ui32 token;
                struct
                {
                    ui32 repeat   : 16;// 0-15
                    ui32 scancode : 9; // 16-24 (24 - extended)
                    ui32 reserved : 5; // 25-29 (29 - context)
                    ui32 state    : 2; // 30-31: 0 - pressed, 1 - repeated, 2 - unknown, 3 - released
                } v;
            };
            auto param = key_state{ .token = (ui32)lParam };
            log("vkey: ", utf::to_hex(vkey),
                " scode: ", utf::to_hex(param.v.scancode),
                " state: ", param.v.state == 0 ? "pressed"
                          : param.v.state == 1 ? "rep"
                          : param.v.state == 3 ? "released" : "unknown");
            if (vkey == 0x1b) manager::close();
            kbs() = keybd_state();
            //auto s = keybd_state();
            //log("keybd ", utf::to_bin(s));
            //static auto keybd_state = std::array<byte, 256>{};
            //::GetKeyboardState(keybd_state.data());
            //auto l_shift = keybd_state[VK_LSHIFT];
            //auto r_shift = keybd_state[VK_RSHIFT];
            //auto l_win   = keybd_state[VK_LWIN];
            //auto r_win   = keybd_state[VK_RWIN];
            //bool alt     = keybd_state[VK_MENU];
            //bool l_alt   = keybd_state[VK_LMENU];
            //bool r_alt   = keybd_state[VK_RMENU];
            //bool l_ctrl  = keybd_state[VK_LCONTROL];
            //bool r_ctrl  = keybd_state[VK_RCONTROL];
            //log("keybd",
            //    "\n\t l_shift ", utf::to_hex(l_shift ),
            //    "\n\t r_shift ", utf::to_hex(r_shift ),
            //    "\n\t l_win   ", utf::to_hex(l_win   ),
            //    "\n\t r_win   ", utf::to_hex(r_win   ),
            //    "\n\t alt     ", utf::to_hex(alt     ),
            //    "\n\t l_alt   ", utf::to_hex(l_alt   ),
            //    "\n\t r_alt   ", utf::to_hex(r_alt   ),
            //    "\n\t l_ctrl  ", utf::to_hex(l_ctrl  ),
            //    "\n\t r_ctrl  ", utf::to_hex(r_ctrl  ));
        }
        void focus_event(bool focused)
        {
            log(focused ? "focused" : "unfocused");
        }
    };
}
// Copyright (c) Dmitry Sapozhnikov
// Licensed under the MIT license.

#pragma once

#include "baseui.hpp"

namespace netxs::events::userland
{
    namespace hids
    {
        EVENTPACK( netxs::events::userland::root::hids )
        {
            EVENT_XS( die      , input::hids ), // release::global: Notify about the mouse controller is gone. Signal to delete gears inside dtvt-objects.
            EVENT_XS( halt     , input::hids ), // release::global: Notify about the mouse controller is outside.
            EVENT_XS( clipboard, input::hids ), // release/request: Set/get clipboard data.
            GROUP_XS( keybd    , input::hids ),
            GROUP_XS( mouse    , input::hids ),
            GROUP_XS( focus    , input::foci ), // Focus related events.
            GROUP_XS( device   , input::hids ), // Primary device event group for fast forwarding.

            SUBSET_XS( keybd )
            {
                GROUP_XS( key, input::hids ),

                SUBSET_XS( key )
                {
                    EVENT_XS( post, input::hids ),
                };
            };
            SUBSET_XS( mouse )
            {
                EVENT_XS( move  , input::hids ),
                GROUP_XS( hover , input::hids ),
                GROUP_XS( scroll, input::hids ),
                GROUP_XS( button, input::hids ),

                SUBSET_XS( hover )
                {
                    EVENT_XS( enter, input::hids ), // Inform the form about the mouse hover.
                    EVENT_XS( leave, input::hids ), // Inform the form about leaving the mouse.
                };
                SUBSET_XS( scroll )
                {
                    EVENT_XS( act, input::hids ),
                };
                SUBSET_XS( button )
                {
                    GROUP_XS( up      , input::hids ),
                    GROUP_XS( down    , input::hids ),
                    GROUP_XS( click   , input::hids ),
                    GROUP_XS( dblclick, input::hids ),
                    GROUP_XS( tplclick, input::hids ),
                    GROUP_XS( drag    , input::hids ),

                    SUBSET_XS( up )
                    {
                        EVENT_XS( left     , input::hids ),
                        EVENT_XS( right    , input::hids ),
                        EVENT_XS( middle   , input::hids ),
                        EVENT_XS( xbutton1 , input::hids ),
                        EVENT_XS( xbutton2 , input::hids ),
                        EVENT_XS( leftright, input::hids ),
                    };
                    SUBSET_XS( down )
                    {
                        EVENT_XS( left     , input::hids ),
                        EVENT_XS( right    , input::hids ),
                        EVENT_XS( middle   , input::hids ),
                        EVENT_XS( xbutton1 , input::hids ),
                        EVENT_XS( xbutton2 , input::hids ),
                        EVENT_XS( leftright, input::hids ),
                    };
                    SUBSET_XS( click )
                    {
                        EVENT_XS( left     , input::hids ),
                        EVENT_XS( right    , input::hids ),
                        EVENT_XS( middle   , input::hids ),
                        EVENT_XS( xbutton1 , input::hids ),
                        EVENT_XS( xbutton2 , input::hids ),
                        EVENT_XS( leftright, input::hids ),

                        INDEX_XS( left, right, middle, xbutton1, xbutton2, leftright ),
                    };
                    SUBSET_XS( dblclick )
                    {
                        EVENT_XS( left     , input::hids ),
                        EVENT_XS( right    , input::hids ),
                        EVENT_XS( middle   , input::hids ),
                        EVENT_XS( xbutton1 , input::hids ),
                        EVENT_XS( xbutton2 , input::hids ),
                        EVENT_XS( leftright, input::hids ),
                    };
                    SUBSET_XS( tplclick )
                    {
                        EVENT_XS( left     , input::hids ),
                        EVENT_XS( right    , input::hids ),
                        EVENT_XS( middle   , input::hids ),
                        EVENT_XS( xbutton1 , input::hids ),
                        EVENT_XS( xbutton2 , input::hids ),
                        EVENT_XS( leftright, input::hids ),
                    };
                    SUBSET_XS( drag )
                    {
                        GROUP_XS( start , input::hids ),
                        GROUP_XS( pull  , input::hids ),
                        GROUP_XS( cancel, input::hids ),
                        GROUP_XS( stop  , input::hids ),

                        SUBSET_XS( start )
                        {
                            EVENT_XS( left     , input::hids ),
                            EVENT_XS( right    , input::hids ),
                            EVENT_XS( middle   , input::hids ),
                            EVENT_XS( xbutton1 , input::hids ),
                            EVENT_XS( xbutton2 , input::hids ),
                            EVENT_XS( leftright, input::hids ),

                            INDEX_XS( left, right, middle, xbutton1, xbutton2, leftright ),
                        };
                        SUBSET_XS( pull )
                        {
                            EVENT_XS( left     , input::hids ),
                            EVENT_XS( right    , input::hids ),
                            EVENT_XS( middle   , input::hids ),
                            EVENT_XS( xbutton1 , input::hids ),
                            EVENT_XS( xbutton2 , input::hids ),
                            EVENT_XS( leftright, input::hids ),

                            INDEX_XS( left, right,  middle, xbutton1, xbutton2, leftright ),
                        };
                        SUBSET_XS( cancel )
                        {
                            EVENT_XS( left     , input::hids ),
                            EVENT_XS( right    , input::hids ),
                            EVENT_XS( middle   , input::hids ),
                            EVENT_XS( xbutton1 , input::hids ),
                            EVENT_XS( xbutton2 , input::hids ),
                            EVENT_XS( leftright, input::hids ),

                            INDEX_XS( left, right, middle, xbutton1, xbutton2, leftright ),
                        };
                        SUBSET_XS( stop )
                        {
                            EVENT_XS( left     , input::hids ),
                            EVENT_XS( right    , input::hids ),
                            EVENT_XS( middle   , input::hids ),
                            EVENT_XS( xbutton1 , input::hids ),
                            EVENT_XS( xbutton2 , input::hids ),
                            EVENT_XS( leftright, input::hids ),

                            INDEX_XS( left, right, middle, xbutton1, xbutton2, leftright ),
                        };
                    };
                };
            };
            SUBSET_XS( focus )
            {
                EVENT_XS( hop, input::foci ), // request: Switch focus branch to seed.item.
                EVENT_XS( cut, input::foci ), // request: Unfocus and delete downstream (to inside) focus route.
                EVENT_XS( add, input::foci ), // request: Initiate focus setting toward outside (used by gui and dtvt).
                EVENT_XS( rem, input::foci ), // request: Initiate focus unsetting toward outside (used by gui and dtvt).
                EVENT_XS( dup, input::foci ), // request: Make a focus tree copy from default.
                GROUP_XS( set, input::foci ), // Release: Set on/off focus toward inside; preview: Set on/off focus toward outside.

                SUBSET_XS( set )
                {
                    EVENT_XS( on , input::foci ), // release: Set focus toward inside; preview: Set focus toward outside.
                    EVENT_XS( off, input::foci ), // release: Unset focus toward inside; preview: Unset focus toward outside.
                };
            };
            SUBSET_XS( device )
            {
                GROUP_XS( mouse, input::hids ), // release: Primary mouse event for fast forwarding.
                GROUP_XS( user , id_t        ), // Device properties.

                SUBSET_XS( mouse )
                {
                    EVENT_XS( on, input::hids ),
                };
                SUBSET_XS( user )
                {
                    EVENT_XS( login , id_t ),
                    EVENT_XS( logout, id_t ),
                };
            };
        };
    }
}

namespace netxs::input
{
    using ui::sptr;
    using ui::wptr;
    using ui::base;
    using ui::face;
    using ui::page;

    namespace events = netxs::events::userland::hids;

    namespace key
    {
        static constexpr auto ExtendedKey = 0x0100;
        static constexpr auto NumLockMode = 0x0020;

        static constexpr auto _counter = __COUNTER__ + 1;
        static constexpr auto released = __COUNTER__ - _counter;
        static constexpr auto pressed  = __COUNTER__ - _counter;
        static constexpr auto repeated = __COUNTER__ - _counter;

        struct map
        {
            sz_t hash; // map: Key hash.

            static auto& mask()
            {
                static auto m = std::vector<si32>(256);
                return m;
            }
            static auto& mask(si32 vk)
            {
                return mask()[std::clamp(vk, 0, 255)];
            }
            static auto& data(si32 keycode)
            {
                struct key
                {
                    view name;
                    view generic;
                    si32 vkey;
                    si32 scan;
                    si32 edit;
                };
                static auto data = std::vector<key>(256);
                return data[std::clamp(keycode, 0, 255)];
            }

            map(si32 vk, si32 sc, si32 cs)
                : hash{ (sz_t)(mask(vk) & (vk | (sc << 8) | (cs << 16))) }
            { }
            map(si32 vk, si32 sc, si32 cs, si32 keymask, view keyname, view generic_keyname, si32 doinput, si32 id)
            {
                mask(vk) = keymask;
                data(id) = { .name = keyname, .generic = generic_keyname, .vkey = vk, .scan = sc, .edit = doinput };
                hash = (sz_t)(keymask & (vk | (sc << 8) | (cs << 16)));
            }

            bool operator == (map const& m) const = default;
            struct hashproc
            {
                auto operator()(map const& m) const
                {
                    return m.hash;
                }
            };
        };

        //todo check non-us kb layouts with key::Slash
        #define key_list \
            /*Id   Index Vkey  Scan    CtrlState          Mask  I  Name              GenericName      */\
            X(0,      0,    0,    0,           0, 0x0000'00'FF, 1, undef           , "undef"           )\
            X(1,      0, 0xFF, 0xFF,           0, 0x0000'FF'FF, 0, config          , "config"          )\
            X(2,   0xA2, 0x11, 0x1D,           0, 0x0100'00'FF, 0, LeftCtrl        , "Ctrl"            )\
            X(3,   0xA3, 0x11, 0x1D, ExtendedKey, 0x0100'00'FF, 0, RightCtrl       , "Ctrl"            )\
            X(4,   0xA4, 0x12, 0x38,           0, 0x0100'00'FF, 0, LeftAlt         , "Alt"             )\
            X(5,   0xA5, 0x12, 0x38, ExtendedKey, 0x0100'00'FF, 0, RightAlt        , "Alt"             )\
            X(6,   0xA0, 0x10, 0x2A,           0, 0x0000'FF'FF, 0, LeftShift       , "Shift"           )\
            X(7,   0xA1, 0x10, 0x36,           0, 0x0000'FF'FF, 0, RightShift      , "Shift"           )\
            X(8,   0x5B, 0x5B, 0x5B, ExtendedKey, 0x0000'00'FF, 0, LeftWin         , "Win"             )\
            X(9,   0x5C, 0x5C, 0x5C, ExtendedKey, 0x0000'00'FF, 0, RightWin        , "Win"             )\
            X(10,  0x5D, 0x5D, 0x5D, ExtendedKey, 0x0000'00'FF, 0, Apps            , "Apps"            )\
            X(12,  0x90, 0x90, 0x45,           0, 0x0000'00'FF, 0, NumLock         , "NumLock"         )\
            X(14,  0x14, 0x14, 0x3A,           0, 0x0000'00'FF, 0, CapsLock        , "CapsLock"        )\
            X(16,  0x91, 0x91, 0x45,           0, 0x0000'00'FF, 0, ScrollLock      , "ScrollLock"      )\
            X(18,  0x1B, 0x1B, 0x01,           0, 0x0000'00'FF, 1, Esc             , "Esc"             )\
            X(20,  0x20, 0x20, 0x39,           0, 0x0000'00'FF, 1, Space           , "Space"           )\
            X(22,  0x08, 0x08, 0x0E,           0, 0x0000'00'FF, 1, Backspace       , "Backspace"       )\
            X(24,  0x09, 0x09, 0x0F,           0, 0x0000'00'FF, 1, Tab             , "Tab"             )\
            X(26,  0x03, 0x03, 0x46,           0, 0x0000'FF'FF, 1, Break           , "Break"           )\
            X(28,  0x13, 0x13, 0x45,           0, 0x0000'FF'FF, 0, Pause           , "Pause"           )\
            X(30,  0x29, 0x29,    0,           0, 0x0000'00'FF, 0, Select          , "Select"          )\
            X(32,  0x2C, 0x2C, 0x54,           0, 0x0000'FF'FF, 1, SysRq           , "SysRq"           )\
            X(34,  0x2C, 0x2C, 0x37, ExtendedKey, 0x0100'FF'FF, 0, PrintScreen     , "PrintScreen"     )\
            X(36,  0x0D, 0x0D, 0x1C,           0, 0x0100'00'FF, 1, KeyEnter        , "Enter"           )\
            X(37,  0x0D, 0x0D, 0x1C, ExtendedKey, 0x0100'00'FF, 1, NumpadEnter     , "Enter"           )\
            X(38,  0x21, 0x21, 0x49, ExtendedKey, 0x0100'00'FF, 1, KeyPageUp       , "PageUp"          )\
            X(39,  0x21, 0x21, 0x49,           0, 0x0100'00'FF, 1, NumpadPageUp    , "PageUp"          )\
            X(40,  0x22, 0x22, 0x51, ExtendedKey, 0x0100'00'FF, 1, KeyPageDown     , "PageDown"        )\
            X(41,  0x22, 0x22, 0x51,           0, 0x0100'00'FF, 1, NumpadPageDown  , "PageDown"        )\
            X(42,  0x23, 0x23, 0x4F, ExtendedKey, 0x0100'00'FF, 1, KeyEnd          , "End"             )\
            X(43,  0x23, 0x23, 0x4F,           0, 0x0100'00'FF, 1, NumpadEnd       , "End"             )\
            X(44,  0x24, 0x24, 0x47, ExtendedKey, 0x0100'00'FF, 1, KeyHome         , "Home"            )\
            X(45,  0x24, 0x24, 0x47,           0, 0x0100'00'FF, 1, NumpadHome      , "Home"            )\
            X(46,  0x25, 0x25, 0x4B, ExtendedKey, 0x0100'00'FF, 1, KeyLeftArrow    , "LeftArrow"       )\
            X(47,  0x25, 0x25, 0x4B,           0, 0x0100'00'FF, 1, NumpadLeftArrow , "LeftArrow"       )\
            X(48,  0x26, 0x26, 0x48, ExtendedKey, 0x0100'00'FF, 1, KeyUpArrow      , "UpArrow"         )\
            X(49,  0x26, 0x26, 0x48,           0, 0x0100'00'FF, 1, NumpadUpArrow   , "UpArrow"         )\
            X(50,  0x27, 0x27, 0x4D, ExtendedKey, 0x0100'00'FF, 1, KeyRightArrow   , "RightArrow"      )\
            X(51,  0x27, 0x27, 0x4D,           0, 0x0100'00'FF, 1, NumpadRightArrow, "RightArrow"      )\
            X(52,  0x28, 0x28, 0x50, ExtendedKey, 0x0100'00'FF, 1, KeyDownArrow    , "DownArrow"       )\
            X(53,  0x28, 0x28, 0x50,           0, 0x0100'00'FF, 1, NumpadDownArrow , "DownArrow"       )\
            X(54,  0x30, 0x30, 0x0B,           0, 0x0000'FF'FF, 1, Key0            , "0"               )\
            X(55,  0x60, 0x60, 0x52, NumLockMode, 0x0000'FF'FF, 1, Numpad0         , "0"               )\
            X(56,  0x31, 0x31, 0x02,           0, 0x0000'FF'FF, 1, Key1            , "1"               )\
            X(57,  0x61, 0x61, 0x4F, NumLockMode, 0x0000'FF'FF, 1, Numpad1         , "1"               )\
            X(58,  0x32, 0x32, 0x03,           0, 0x0000'FF'FF, 1, Key2            , "2"               )\
            X(59,  0x62, 0x62, 0x50, NumLockMode, 0x0000'FF'FF, 1, Numpad2         , "2"               )\
            X(60,  0x33, 0x33, 0x04,           0, 0x0000'FF'FF, 1, Key3            , "3"               )\
            X(61,  0x63, 0x63, 0x51, NumLockMode, 0x0000'FF'FF, 1, Numpad3         , "3"               )\
            X(62,  0x34, 0x34, 0x05,           0, 0x0000'FF'FF, 1, Key4            , "4"               )\
            X(63,  0x64, 0x64, 0x4B, NumLockMode, 0x0000'FF'FF, 1, Numpad4         , "4"               )\
            X(64,  0x35, 0x35, 0x06,           0, 0x0000'FF'FF, 1, Key5            , "5"               )\
            X(65,  0x65, 0x65, 0x4C, NumLockMode, 0x0000'FF'FF, 1, Numpad5         , "5"               )\
            X(66,  0x36, 0x36, 0x07,           0, 0x0000'FF'FF, 1, Key6            , "6"               )\
            X(67,  0x66, 0x66, 0x4D, NumLockMode, 0x0000'FF'FF, 1, Numpad6         , "6"               )\
            X(68,  0x37, 0x37, 0x08,           0, 0x0000'FF'FF, 1, Key7            , "7"               )\
            X(69,  0x67, 0x67, 0x47, NumLockMode, 0x0000'FF'FF, 1, Numpad7         , "7"               )\
            X(70,  0x38, 0x38, 0x09,           0, 0x0000'FF'FF, 1, Key8            , "8"               )\
            X(71,  0x68, 0x68, 0x48, NumLockMode, 0x0000'FF'FF, 1, Numpad8         , "8"               )\
            X(72,  0x39, 0x39, 0x0A,           0, 0x0000'FF'FF, 1, Key9            , "9"               )\
            X(73,  0x69, 0x69, 0x49, NumLockMode, 0x0000'FF'FF, 1, Numpad9         , "9"               )\
            X(74,  0x2D, 0x2D, 0x52, ExtendedKey, 0x0100'00'FF, 1, KeyInsert       , "Insert"          )\
            X(75,  0x2D, 0x2D, 0x52,           0, 0x0100'00'FF, 1, NumpadInsert    , "Insert"          )\
            X(76,  0x2E, 0x2E, 0x53, ExtendedKey, 0x0100'00'FF, 1, KeyDelete       , "Delete"          )\
            X(77,  0x2E, 0x2E, 0x55,           0, 0x0100'00'FF, 1, NumpadDelete    , "Delete"          )\
            X(78,  0x0C, 0x0C, 0x4C, ExtendedKey, 0x0100'00'FF, 1, KeyClear        , "Clear"           )\
            X(79,  0x0C, 0x0C, 0x4C,           0, 0x0100'00'FF, 1, NumpadClear     , "Clear"           )\
            X(80,  0x6A, 0x6A, 0x09,           0, 0x0000'FF'FF, 1, KeyMultiply     , "*"               )\
            X(81,  0x6A, 0x6A, 0x37,           0, 0x0000'FF'FF, 1, NumpadMultiply  , "*"               )\
            X(82,  0x6B, 0x6B, 0x0D,           0, 0x0000'FF'FF, 1, KeyPlus         , "Plus"            )\
            X(83,  0x6B, 0x6B, 0x4E,           0, 0x0000'FF'FF, 1, NumpadPlus      , "Plus"            )\
            X(84,  0x6C, 0x6C,    0,           0, 0x0020'00'FF, 1, KeySeparator    , "Separator"       )\
            X(85,  0x6C, 0x6C,    0, NumLockMode, 0x0020'00'FF, 1, NumpadSeparator , "Separator"       )\
            X(86,  0xBD, 0xBD, 0x0C,           0, 0x0000'00'FF, 1, KeyMinus        , "Minus"           )\
            X(87,  0x6D, 0x6D, 0x4A,           0, 0x0000'00'FF, 1, NumpadMinus     , "Minus"           )\
            X(88,  0xBE, 0xBE, 0x34,           0, 0x0000'00'FF, 1, KeyPeriod       , "."               )\
            X(89,  0x6E, 0x6E, 0x53, NumLockMode, 0x0000'00'FF, 1, NumpadDecimal   , "."               )\
            X(90,  0xBF, 0xBF, 0x35,           0, 0x0000'00'FF, 1, KeySlash        , "/"               )\
            X(91,  0x6F, 0x6F, 0x35, ExtendedKey, 0x0000'00'FF, 1, NumpadSlash     , "/"               )\
            X(92,  0xDC, 0xDC, 0x2B,           0, 0x0000'00'FF, 1, BackSlash       , "BackSlash"       )\
            X(94,  0xDB, 0xDB, 0x1A,           0, 0x0000'00'FF, 1, OpenBracket     , "["               )\
            X(96,  0xDD, 0xDD, 0x1B,           0, 0x0000'00'FF, 1, ClosedBracket   , "]"               )\
            X(98,  0xBB, 0xBB, 0x0D,           0, 0x0000'00'FF, 1, Equal           , "="               )\
            X(100, 0xC0, 0xC0, 0x29,           0, 0x0000'00'FF, 1, BackQuote       , "`"               )\
            X(102, 0xDE, 0xDE, 0x28,           0, 0x0000'00'FF, 1, SingleQuote     , "'"               )\
            X(104, 0xBC, 0xBC, 0x33,           0, 0x0000'00'FF, 1, Comma           , ","               )\
            X(106, 0xBA, 0xBA, 0x27,           0, 0x0000'00'FF, 1, Semicolon       , ";"               )\
            X(108, 0x70, 0x70, 0x3B,           0, 0x0000'00'FF, 1, F1              , "F1"              )\
            X(110, 0x71, 0x71, 0x3C,           0, 0x0000'00'FF, 1, F2              , "F2"              )\
            X(112, 0x72, 0x72, 0x3D,           0, 0x0000'00'FF, 1, F3              , "F3"              )\
            X(114, 0x73, 0x73, 0x3E,           0, 0x0000'00'FF, 1, F4              , "F4"              )\
            X(116, 0x74, 0x74, 0x3F,           0, 0x0000'00'FF, 1, F5              , "F5"              )\
            X(118, 0x75, 0x75, 0x40,           0, 0x0000'00'FF, 1, F6              , "F6"              )\
            X(120, 0x76, 0x76, 0x41,           0, 0x0000'00'FF, 1, F7              , "F7"              )\
            X(122, 0x77, 0x77, 0x42,           0, 0x0000'00'FF, 1, F8              , "F8"              )\
            X(124, 0x78, 0x78, 0x43,           0, 0x0000'00'FF, 1, F9              , "F9"              )\
            X(126, 0x79, 0x79, 0x44,           0, 0x0000'00'FF, 1, F10             , "F10"             )\
            X(128, 0x7A, 0x7A, 0x57,           0, 0x0000'00'FF, 1, F11             , "F11"             )\
            X(130, 0x7B, 0x7B, 0x5B,           0, 0x0000'00'FF, 1, F12             , "F12"             )\
            X(132, 0x7C, 0x7C,    0,           0, 0x0000'00'FF, 1, F13             , "F13"             )\
            X(134, 0x7D, 0x7D,    0,           0, 0x0000'00'FF, 1, F14             , "F14"             )\
            X(136, 0x7E, 0x7E,    0,           0, 0x0000'00'FF, 1, F15             , "F15"             )\
            X(138, 0x7F, 0x7F,    0,           0, 0x0000'00'FF, 1, F16             , "F16"             )\
            X(140, 0x80, 0x80,    0,           0, 0x0000'00'FF, 1, F17             , "F17"             )\
            X(142, 0x81, 0x81,    0,           0, 0x0000'00'FF, 1, F18             , "F18"             )\
            X(144, 0x82, 0x82,    0,           0, 0x0000'00'FF, 1, F19             , "F19"             )\
            X(146, 0x83, 0x83,    0,           0, 0x0000'00'FF, 1, F20             , "F20"             )\
            X(148, 0x84, 0x84,    0,           0, 0x0000'00'FF, 1, F21             , "F21"             )\
            X(150, 0x85, 0x85,    0,           0, 0x0000'00'FF, 1, F22             , "F22"             )\
            X(152, 0x86, 0x86,    0,           0, 0x0000'00'FF, 1, F23             , "F23"             )\
            X(154, 0x87, 0x87,    0,           0, 0x0000'00'FF, 1, F24             , "F24"             )\
            X(156, 0x41, 0x41,    0,           0, 0x0100'00'FF, 1, KeyA            , "A"               )\
            X(158, 0x42, 0x42,    0,           0, 0x0100'00'FF, 1, KeyB            , "B"               )\
            X(160, 0x43, 0x43,    0,           0, 0x0100'00'FF, 1, KeyC            , "C"               )\
            X(162, 0x44, 0x44,    0,           0, 0x0100'00'FF, 1, KeyD            , "D"               )\
            X(164, 0x45, 0x45,    0,           0, 0x0100'00'FF, 1, KeyE            , "E"               )\
            X(166, 0x46, 0x46,    0,           0, 0x0100'00'FF, 1, KeyF            , "F"               )\
            X(168, 0x47, 0x47,    0,           0, 0x0100'00'FF, 1, KeyG            , "G"               )\
            X(170, 0x48, 0x48,    0,           0, 0x0100'00'FF, 1, KeyH            , "H"               )\
            X(172, 0x49, 0x49,    0,           0, 0x0100'00'FF, 1, KeyI            , "I"               )\
            X(174, 0x4A, 0x4A,    0,           0, 0x0100'00'FF, 1, KeyJ            , "J"               )\
            X(176, 0x4B, 0x4B,    0,           0, 0x0100'00'FF, 1, KeyK            , "K"               )\
            X(178, 0x4C, 0x4C,    0,           0, 0x0100'00'FF, 1, KeyL            , "L"               )\
            X(180, 0x4D, 0x4D,    0,           0, 0x0100'00'FF, 1, KeyM            , "M"               )\
            X(182, 0x4E, 0x4E,    0,           0, 0x0100'00'FF, 1, KeyN            , "N"               )\
            X(184, 0x4F, 0x4F,    0,           0, 0x0100'00'FF, 1, KeyO            , "O"               )\
            X(186, 0x50, 0x50,    0,           0, 0x0100'00'FF, 1, KeyP            , "P"               )\
            X(188, 0x51, 0x51,    0,           0, 0x0100'00'FF, 1, KeyQ            , "Q"               )\
            X(190, 0x52, 0x52,    0,           0, 0x0100'00'FF, 1, KeyR            , "R"               )\
            X(192, 0x53, 0x53,    0,           0, 0x0100'00'FF, 1, KeyS            , "S"               )\
            X(194, 0x54, 0x54,    0,           0, 0x0100'00'FF, 1, KeyT            , "T"               )\
            X(196, 0x55, 0x55,    0,           0, 0x0100'00'FF, 1, KeyU            , "U"               )\
            X(198, 0x56, 0x56,    0,           0, 0x0100'00'FF, 1, KeyV            , "V"               )\
            X(200, 0x57, 0x57,    0,           0, 0x0100'00'FF, 1, KeyW            , "W"               )\
            X(202, 0x58, 0x58,    0,           0, 0x0100'00'FF, 1, KeyX            , "X"               )\
            X(204, 0x59, 0x59,    0,           0, 0x0100'00'FF, 1, KeyY            , "Y"               )\
            X(206, 0x5A, 0x5A,    0,           0, 0x0100'00'FF, 1, KeyZ            , "Z"               )\
            X(208, 0x5F, 0x5F,    0, ExtendedKey, 0x0100'00'FF, 0, Sleep           , "Sleep"           )\
            X(210, 0xB7, 0xB7,    0, ExtendedKey, 0x0100'00'FF, 0, Calculator      , "Calculator"      )\
            X(212, 0x48, 0x48,    0, ExtendedKey, 0x0100'00'FF, 0, Mail            , "Mail"            )\
            X(214, 0xAD, 0xAD,    0, ExtendedKey, 0x0100'00'FF, 0, MediaVolMute    , "MediaVolMute"    )\
            X(216, 0xAE, 0xAE,    0, ExtendedKey, 0x0100'00'FF, 0, MediaVolDown    , "MediaVolDown"    )\
            X(218, 0xAF, 0xAF,    0, ExtendedKey, 0x0100'00'FF, 0, MediaVolUp      , "MediaVolUp"      )\
            X(220, 0xB0, 0xB0,    0, ExtendedKey, 0x0100'00'FF, 0, MediaNext       , "MediaNext"       )\
            X(222, 0xB1, 0xB1,    0, ExtendedKey, 0x0100'00'FF, 0, MediaPrev       , "MediaPrev"       )\
            X(224, 0xB2, 0xB2,    0, ExtendedKey, 0x0100'00'FF, 0, MediaStop       , "MediaStop"       )\
            X(226, 0xB3, 0xB3,    0, ExtendedKey, 0x0100'00'FF, 0, MediaPlayPause  , "MediaPlayPause"  )\
            X(228, 0xB5, 0xB5,    0, ExtendedKey, 0x0100'00'FF, 0, MediaSelect     , "MediaSelect"     )\
            X(230, 0xA6, 0xA6,    0, ExtendedKey, 0x0100'00'FF, 0, BrowserBack     , "BrowserBack"     )\
            X(232, 0xA7, 0xA7,    0, ExtendedKey, 0x0100'00'FF, 0, BrowserForward  , "BrowserForward"  )\
            X(234, 0xA8, 0xA8,    0, ExtendedKey, 0x0100'00'FF, 0, BrowserRefresh  , "BrowserRefresh"  )\
            X(236, 0xA9, 0xA9,    0, ExtendedKey, 0x0100'00'FF, 0, BrowserStop     , "BrowserStop"     )\
            X(238, 0xAA, 0xAA,    0, ExtendedKey, 0x0100'00'FF, 0, BrowserSearch   , "BrowserSearch"   )\
            X(240, 0xAB, 0xAB,    0, ExtendedKey, 0x0100'00'FF, 0, BrowserFavorites, "BrowserFavorites")\
            X(242, 0xAC, 0xAC,    0, ExtendedKey, 0x0100'00'FF, 0, BrowserHome     , "BrowserHome"     )

        #define X(KeyId, Index, Vkey, Scan, CtrlState, Mask, Input, Name, GenericName) \
            static constexpr auto Name = KeyId;
            key_list
        #undef X

        static const auto keymap = std::unordered_map<map, si32, map::hashproc>
        {
            #define X(KeyId, Index, Vkey, Scan, CtrlState, Mask, Input, Name, GenericName) \
                { map{ Vkey, Scan, CtrlState, Mask, #Name, GenericName, Input, Name }, Name },
                key_list
            #undef X
        };

        static const auto specific_names = std::unordered_map<text, si32, qiew::hash, qiew::equal>
        {
            #define X(KeyId, Index, Vkey, Scan, CtrlState, Mask, Input, Name, GenericName) \
                { utf::to_lower(#Name), KeyId },
                key_list
            #undef X
        };
        static const auto generic_names = std::unordered_map<text, si32, qiew::hash, qiew::equal>
        {
            #define X(KeyId, Index, Vkey, Scan, CtrlState, Mask, Input, Name, GenericName) \
                { utf::to_lower(GenericName), KeyId & -2 },
                key_list
            #undef X
        };

        #undef key_list

        struct kmap
        {
            struct chord_item_t
            {
                si32 index;
                si32 scode;
                time stamp;
            };

            using cmap = std::map<si32, chord_item_t>; // std::unordered_map doesn't sort items.

            cmap pushed{}; // kmap: Pushed key map.
            bool keyout{}; // kmap: Some key has left the key chord.

            void reset(auto& k)
            {
                k.vkchord.clear();
                k.scchord.clear();
                k.chchord.clear();
                pushed.clear();
                keyout = {};
            }
            auto exist(si32 keyid)
            {
                auto iter = pushed.find(keyid);
                return iter != pushed.end();
            }
            // Build key chords.
            // key chord is a set of 16-bit words: 0x000a 0x000b ... 0xffff 0xa 0xfe0e
            //  15 bit: 0 - virt code, 1 - scan code ('\x80').
            //  14 bit: 0 - pressed, 1 - released ('\x40').
            //  13 bit: 1 - all subsequent bytes form a grapheme cluster ('\x20').
            //  0-12 bits: virt or scan code. For clusters it is set to '\x0FFF'.
            static void push_keyid(bool is_pressed, text& vkchord, si32 keyid)
            {
                vkchord.push_back(is_pressed ? '\x00' : '\x40');
                vkchord.push_back((byte)keyid);
            }
            static void push_scode(bool is_pressed, text& scchord, si32 scode)
            {
                scchord.push_back((byte)((is_pressed ? 0x00 : 0x40) | 0x80 | ((scode >> 8) & 0x01)));
                scchord.push_back((byte)(scode & 0xFF));
            }
            static void push_cluster(bool is_pressed, text& chchord, view cluster)
            {
                chchord += (is_pressed ? '\x00' : '\x40') | '\x20';
                chchord += '\xFF';
                chchord += cluster;
            }
            template<class P = noop>
            void build(auto& k, P test_key_released = {})
            {
                if (k.keystat != input::key::repeated)
                {
                    //log("key=%% pressed=%%", input::key::map::data(k.keycode).name, k.keystat);
                    if (k.keystat == input::key::released)
                    {
                        pushed.erase(k.keycode);
                    }
                    k.vkchord.clear();
                    k.scchord.clear();
                    k.chchord.clear();
                    auto vk_valid = k.keycode > input::key::config;
                    auto sc_valid = k.scancod > 0;
                    if (!keyout || k.keystat != input::key::released)
                    {
                        keyout = k.keystat == input::key::released;
                        //log(" erasing %%", k.keystat == input::key::released ? "key::released" : k.keystat == input::key::pressed ? "key::pressed" : "key::repeated");
                        std::erase_if(pushed, [&](auto& rec)
                        {
                            auto& [keyid, val] = rec;
                            //log("\tcheck keyid=%%", input::key::map::data(keyid).name);
                            auto is_released = test_key_released(val.index); // Check if it is still pressed.
                            if (!is_released && keyid != k.keycode/*exclude repeated key*/)
                            {
                                if (keyid <= input::key::config) vk_valid = faux;
                                if (val.scode == 0) sc_valid = faux;
                                push_keyid(true, k.vkchord, keyid);
                                push_scode(true, k.scchord, val.scode);
                            }
                            //else if (is_released) log("\tkeyid=%% released", input::key::map::data(keyid).name);
                            return is_released;
                        });
                        auto sign = !!k.keystat;
                        if (vk_valid && k.cluster.size() && k.cluster.front() != '\0')
                        {
                            k.chchord = k.vkchord;
                            push_cluster(sign, k.chchord, k.cluster);
                        }
                        push_keyid(sign, k.vkchord, k.keycode);
                        push_scode(sign, k.scchord, k.scancod | (k.extflag ? 0x100 : 0));
                        if (!vk_valid) k.vkchord.clear();
                        if (!sc_valid) k.scchord.clear();
                    }
                    if (k.keystat == input::key::pressed)
                    {
                        auto& key = pushed[k.keycode];
                        key.scode = k.scancod | (k.extflag ? 0x100 : 0); // Store the scan code of a pressed key.
                        key.index = k.virtcod; // Store the virtual code to check later that it is still pressed.
                        key.stamp = datetime::now();
                    }
                }
            }
            static auto to_string(qiew chord, bool generic)
            {
                auto crop = text{};
                while (chord.size() > 1)
                {
                    auto s = (byte)chord.pop_front();
                    auto v = (byte)chord.pop_front();
                    if (crop.size() || s & 0x40) crop += s & 0x40 ? '-' : '+';
                    if (s & 0x80) // Scancodes.
                    {
                        auto value = v | (s & 0x01 ? 0x100 : 0);
                        auto length = value & 0xF00 ? 3 : 2;
                        crop += "0x" + utf::to_hex<true>(value, length);
                    }
                    else if (s & 0x20) // Cluster.
                    {
                        auto plain = utf::debase<faux, faux>(chord);
                        utf::replace_all(plain, "'", "\\'");
                        crop += '\'' + plain + '\'';
                        chord.clear();
                    }
                    else // Keyids
                    {
                        crop += generic ? input::key::map::data(v).generic : input::key::map::data(v).name;
                    }
                }
                return crop;
            }
            static constexpr auto any_key = qiew{ "\0"sv };
            static auto chord_list(qiew chord)
            {
                struct key_t
                {
                    byte sign;
                    si32 code1;
                    si32 code2;
                    text utf8;
                    auto is_scancode() const { return sign & 0x80; }
                    auto is_pressed() const { return !(sign & 0x40); }
                    auto is_cluster() const { return sign & 0x20; }
                };
                auto keys = std::vector<key_t>{};
                auto crop = std::vector<text>{};
                if (utf::to_lower(chord) == "any")
                {
                    crop.push_back(any_key);
                    return crop;
                }
                auto take = [](qiew& chord)
                {
                    auto k = key_t{};
                    utf::trim(chord);
                    if (chord.empty()) return k;
                    auto c = chord.front();
                    if (c != '-') // Is pressed.
                    {
                        if (c == '+')
                        {
                            chord.pop_front(); // Pop '+'.
                            utf::trim(chord);
                            if (chord.empty()) return k;
                            c = chord.front();
                        }
                    }
                    else if (chord.size() > 1)
                    {
                        k.sign |= 0x40;
                        chord.pop_front(); // Pop '-'.
                        utf::trim(chord);
                        if (chord.empty()) return k;
                        c = chord.front();
                    }
                    utf::trim(chord);
                    if (chord.empty()) return k;
                    auto is_scancode = chord.starts_with("0x") || chord.starts_with("0X");
                    if (is_scancode)
                    {
                        chord.remove_prefix(2);
                        if (auto v = utf::to_int<si32, 16>(chord))
                        {
                            k.sign |= 0x80;
                            k.code1 = v.value();
                        }
                    }
                    else if (chord.size() > 2 && chord.front() == chord.back() && (chord.front() == '\'' || chord.front() == '\"')) // The literal key must be the last key in a sequence.
                    {
                        k.sign |= 0x20;
                        k.utf8 = utf::unescape(chord.substr(1, chord.size() - 2));
                        k.code1 = 0xFF;
                        chord.clear();
                    }
                    else if (auto key_name = qiew{ utf::get_word(chord, "+- ") })
                    {
                        auto name = utf::to_lower(key_name);
                        auto iter = input::key::generic_names.find(name);
                        if (iter == input::key::generic_names.end()) // Is specific.
                        {
                            auto iter2 = input::key::specific_names.find(name);
                            if (iter2 != input::key::specific_names.end())
                            {
                                k.code1 = iter2->second;
                            }
                        }
                        else // Is generic.
                        {
                            auto code = iter->second & -2;
                            auto n1 = input::key::map::data(code).name.size();
                            auto n2 = input::key::map::data(code + 1).name.size();
                            k.code1 = n1 ? code : 0;
                            k.code2 = n2 ? code + 1 : 0;
                        }
                    }
                    utf::trim(chord);
                    return k;
                };
                // Split.
                while (chord)
                {
                    auto k = take(chord); // Unfold.
                    if (!k.code1) return crop; // Unknown key.
                    keys.push_back(k);
                }
                if (keys.empty() || keys.size() > 8) return crop; // A maximum of eight keys are allowed per chord.
                // Sort all but last.
                std::sort(keys.begin(), std::prev(keys.end()), [](auto& l, auto& r){ return l.code1 < r.code1; });
                // Generate.
                auto count = 1 << keys.size();
                auto temp = text{};
                while (count--)
                {
                    auto bits = count;
                    for (auto& k : keys)
                    {
                        if (auto code = bits & 1 ? k.code1 : k.code2)
                        {
                            auto sign = k.is_pressed();
                            if (k.is_scancode())
                            {
                                push_scode(sign, temp, code);
                            }
                            else if (k.is_cluster())
                            {
                                push_cluster(sign, temp, k.utf8);
                                break;
                            }
                            else
                            {
                                push_keyid(sign, temp, code);
                            }
                        }
                        else
                        {
                            temp.clear();
                            break;
                        }
                        bits >>= 1;
                    }
                    if (temp.size()) crop.push_back(temp);
                    temp.clear();
                }
                return crop;
            }
        };

        struct keybind_t
        {
            text chord;
            bool preview{};
            std::vector<netxs::sptr<text>> scripts;
        };
        using keybind_list_t = std::vector<keybind_t>;

        template<class ...Args>
        auto xlat(Args&&... args)
        {
            auto iter = keymap.find(map{ args... });
            return iter != keymap.end() ? iter->second : key::undef;
        }
        auto find(si32 vkey, si32 fallback)
        {
            auto k = fallback;
            for (auto& [key, val] : keymap)
            {
                if ((si32)(key.hash & 0xff) == vkey)
                {
                    k = val & -2; // Generic keys only.
                    break;
                }
            }
            return k;
        }
    }

    struct foci
    {
        id_t gear_id{}; // foci: Gear id.
        si32 focus_type{}; // foci: Exclusive focus request.
        bool just_activate_only{}; // foci: Ignore focusable object, just activate it.
        sptr item{}; // foci: Next focused item.
        sptr next{}; // foci: Next focused item.
        si64 treeid{}; // foci: Focus tree id.
        ui64 digest{}; // foci: Incrementing event number to avoid refocusing when connecting recursively.
    };

    using multihome_t = std::pair<wptr, wptr>;

    // input: Mouse tracker.
    struct mouse
    {
        enum mode
        {
            none = 0,
            bttn = 1 << 0,
            drag = 1 << 1,
            move = 1 << 2,
            over = 1 << 3,
            utf8 = 1 << 4,
            buttons_press = bttn,
            buttons_drags = bttn | drag,
            all_movements = bttn | drag | move,
            negative_args = bttn | drag | move | over,
        };
        enum prot
        {
            x11,
            sgr,
            w32,
        };
        enum buttons
        {
            left      = input::events::mouse::button::click::left     .index(),
            right     = input::events::mouse::button::click::right    .index(),
            middle    = input::events::mouse::button::click::middle   .index(),
            xbutton1  = input::events::mouse::button::click::xbutton1 .index(),
            xbutton2  = input::events::mouse::button::click::xbutton2 .index(),
            leftright = input::events::mouse::button::click::leftright.index(),
            numofbuttons,
        };

        struct stat
        {
            enum
            {
                ok,
                halt,
                die,
            };
        };

        struct knob_t
        {
            bool pressed; // knod: Button pressed state.
            bool dragged; // knod: The button is in a dragging state.
            bool blocked; // knod: The button is blocked (leftright disables left and right).
            fp2d clickxy; // knob: Press coordinates.
        };
        struct hist_t // Timer for successive double-clicks, e.g. triple-clicks.
        {
            time fired; // hist: .
            twod coord; // hist: .
            si32 count; // hist: .
        };

        using hist = std::array<hist_t, numofbuttons>;
        using knob = std::array<knob_t, numofbuttons>;
        using tail = netxs::datetime::tail<fp2d>;

        static constexpr auto dragstrt = input::events::mouse::button::drag::start:: any.group<numofbuttons>();
        static constexpr auto dragpull = input::events::mouse::button::drag::pull::  any.group<numofbuttons>();
        static constexpr auto dragcncl = input::events::mouse::button::drag::cancel::any.group<numofbuttons>();
        static constexpr auto dragstop = input::events::mouse::button::drag::stop::  any.group<numofbuttons>();
        static constexpr auto released = input::events::mouse::button::up::          any.group<numofbuttons>();
        static constexpr auto pushdown = input::events::mouse::button::down::        any.group<numofbuttons>();
        static constexpr auto sglclick = input::events::mouse::button::click::       any.group<numofbuttons>();
        static constexpr auto dblclick = input::events::mouse::button::dblclick::    any.group<numofbuttons>();
        static constexpr auto tplclick = input::events::mouse::button::tplclick::    any.group<numofbuttons>();
        static constexpr auto wheeling = input::events::mouse::scroll::act.id;
        static constexpr auto movement = input::events::mouse::move.id;
        static constexpr auto noactive = si32{ -1 };
        static constexpr auto drag_threshold = 0.3f; // mouse: Mouse drag threshold (to support jittery clicks).

        fp2d prime{}; // mouse: System mouse cursor coordinates.
        fp2d coord{}; // mouse: Relative mouse cursor coordinates.
        fp2d click{}; // mouse: Click position on drag start. Should be used in area selection (not dragging).
        fp32 accum{}; // mouse: Mouse motion accumulator to delay mouse drag.
        tail delta{}; // mouse: History of mouse movements for a specified period of time.
        bool reach{}; // mouse: Has the event tree relay reached the mouse event target.
        bool nodbl{}; // mouse: Whether single click event processed (to prevent double clicks).
        bool hzwhl{}; // mouse: If true: Horizontal scrolling. If faux: Vertical scrolling.
        fp32 whlfp{}; // mouse: Scroll delta in float units (lines).
        si32 whlsi{}; // mouse: Scroll delta in integer units (lines).
        si32 locks{}; // mouse: State of the captured buttons (bit field).
        si32 index{}; // mouse: Index of the active button. -1 if the buttons are not involed.
        id_t swift{}; // mouse: Delegate's ID of the current mouse owner.
        id_t hover{}; // mouse: Hover control ID.
        id_t start{}; // mouse: Initiator control ID.
        hint cause{}; // mouse: Current event id.
        hist stamp{}; // mouse: Recorded intervals between successive button presses to track double-clicks.
        span delay{}; // mouse: Double-click threshold.
        knob bttns{}; // mouse: Extended state of mouse buttons.
        sysmouse m_sys{}; // mouse: Device state.
        sysmouse m_sav{}; // mouse: Previous device state.

        si32 pressed_count{}; // mouse: The number of pressed physical buttons.

        // mouse: Forward the extended mouse event.
        virtual void fire(hint cause, si32 index = mouse::noactive) = 0;
        // mouse: Try to forward the mouse event intact.
        virtual bool fire_fast() = 0;

        // mouse: Forward the specified button event.
        template<class T>
        void fire(T const& event_subset, si32 index)
        {
            fire(event_subset[index], index);
        }
        // mouse: Pack the button state into a bitset.
        auto take_button_state()
        {
            auto bitstat = si32{};
            auto pressed = 1 << 0;
            auto dragged = 1 << 1;
            auto blocked = 1 << 2;
            for (auto& b : bttns)
            {
                if (b.pressed) bitstat |= pressed;
                if (b.dragged) bitstat |= dragged;
                if (b.blocked) bitstat |= blocked;
                pressed <<= 3;
                dragged <<= 3;
                blocked <<= 3;
            }
            return bitstat;
        }
        // mouse: Load the button state from a bitset.
        auto load_button_state(si32 bitstat)
        {
            auto pressed = 1 << 0;
            auto dragged = 1 << 1;
            auto blocked = 1 << 2;
            for (auto& b : bttns)
            {
                b.pressed = bitstat & pressed;
                b.dragged = bitstat & dragged;
                b.blocked = bitstat & blocked;
                pressed <<= 3;
                dragged <<= 3;
                blocked <<= 3;
            }
        }
        // mouse: Sync the button state with a bitset.
        auto sync_button_state(si32 bitstat)
        {
            for (auto& b : bttns)
            {
                b.pressed = bitstat & 0x1;
                bitstat >>= 1;
            }
        }
        // mouse: Return a kinetic animator.
        template<class Law>
        auto fader(span spell)
        {
            //todo use current item's type: Law<twod>
            return delta.fader<Law>(spell);
        }
        // mouse: Generate mouse event.
        void update(sysmouse& m, core const& idmap)
        {
            auto button_state = m.buttons;
            pressed_count = 0;
            while (button_state)
            {
                pressed_count += button_state & 0x1;
                button_state >>= 1;
            }
            auto m_buttons = std::bitset<8>(m.buttons);
            // Interpret button combinations.
            //todo possible bug in Apple's Terminal - it does not return the second release
            //                                        in case when two buttons are pressed.
            m_buttons[leftright] = (bttns[leftright].pressed && (m_buttons[left] || m_buttons[right]))
                                                             || (m_buttons[left] && m_buttons[right]);
            m.buttons = (si32)m_buttons.to_ulong();
            auto modschanged = m_sys.ctlstat != m.ctlstat;
            m_sys.set(m);
            auto busy = captured();
            if (busy && fire_fast())
            {
                delta.set(m_sys.coordxy - prime);
                coord = m_sys.coordxy;
                prime = m_sys.coordxy;
                fire(movement); // Update mouse enter/leave state.
                sync_button_state(m_sys.buttons);
                return;
            }
            if (m_buttons[leftright]) // Cancel left and right dragging if it is.
            {
                if (bttns[left].dragged)
                {
                    bttns[left].dragged = faux;
                    fire(dragcncl, left);
                }
                if (bttns[right].dragged)
                {
                    bttns[right].dragged = faux;
                    fire(dragcncl, right);
                }
                m_buttons[left ] = faux;
                m_buttons[right] = faux;
                m_sys.buttons = (si32)m_buttons.to_ulong();
            }

            // Suppress left and right to avoid single button tracking (click, pull, etc)
            bttns[left ].blocked = m_buttons[leftright] || bttns[leftright].pressed;
            bttns[right].blocked = bttns[left].blocked;

            if (m_sys.coordxy != prime || modschanged)
            {
                auto step = m_sys.coordxy - prime;
                if (m.buttons) accum += std::abs(step.x) + std::abs(step.y);
                else           accum = {};
                auto new_target = idmap.link(m_sys.coordxy) != idmap.link(prime);
                auto allow_drag = accum > drag_threshold || new_target;
                delta.set(step);
                auto active = si32{};
                auto genptr = std::begin(bttns);
                for (auto i = 0; i < numofbuttons; i++)
                {
                    auto& genbtn = *genptr++;
                    if (genbtn.pressed && !genbtn.blocked)
                    {
                        if (allow_drag && !genbtn.dragged)
                        {
                            click = genbtn.clickxy;
                            fire(dragstrt, i);
                            genbtn.dragged = true;
                        }
                        active |= 1 << i;
                    }
                }
                coord = m_sys.coordxy;
                prime = m_sys.coordxy;
                if (allow_drag) for (auto i = 0; active; ++i)
                {
                    if (active & 0x1)
                    {
                        fire(dragpull, i);
                    }
                    active >>= 1;
                }
                fire(movement);
            }

            if (!busy && fire_fast())
            {
                sync_button_state(m_sys.buttons);
                return;
            }

            auto genptr = std::begin(bttns);
            for (auto i = 0; i < numofbuttons; i++)
            {
                auto& genbtn = *genptr++;
                auto  sysbtn = m_buttons[i];
                if (genbtn.pressed != sysbtn)
                {
                    genbtn.pressed = sysbtn;
                    if (genbtn.pressed)
                    {
                        genbtn.clickxy = prime;
                        fire(pushdown, i);
                    }
                    else
                    {
                        if (genbtn.dragged)
                        {
                            genbtn.dragged = faux;
                            fire(dragstop, i);
                        }
                        else
                        {
                            if (!genbtn.blocked)
                            {
                                fire(sglclick, i);
                            }
                            if (!nodbl)
                            {
                                // Fire a double/triple-click if delay is not expired
                                // and the mouse is at the same position.
                                auto& s = stamp[i];
                                auto fired = m_sys.timecod;
                                if (fired - s.fired < delay && s.coord == coord)
                                {
                                    if (!genbtn.blocked)
                                    {
                                        if (s.count == 1)
                                        {
                                            fire(dblclick, i);
                                            s.fired = fired;
                                            s.count++;
                                        }
                                        else if (s.count >= 2)
                                        {
                                            fire(tplclick, i);
                                            if (s.count == 4) // Limit to quintuple click.
                                            {
                                                s.fired = {};
                                                s.count = {};
                                            }
                                            else
                                            {
                                                s.fired = fired;
                                                s.count++;
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    s.fired = fired;
                                    s.coord = coord;
                                    s.count = 1;
                                }
                            }
                        }
                        fire(released, i);
                    }
                }
            }

            coord = m_sys.coordxy;
            if (m_sys.wheelfp)
            {
                hzwhl = m_sys.hzwheel;
                whlfp = m_sys.wheelfp;
                whlsi = m_sys.wheelsi;
                fire(wheeling);
                m_sys.hzwheel = {}; // Clear one-shot events.
                m_sys.wheelfp = {};
                m_sys.wheelsi = {};
            }
        }
        // mouse: Return the number of clicks for the specified button.
        auto clicks(si32 button)
        {
            button = std::clamp(button, 0, buttons::numofbuttons - 1);
            return stamp[button].count + 1;
        }
        // mouse: Initiator of visual tree informing about mouse enters/leaves.
        template<bool Entered>
        bool direct(id_t asker)
        {
            if constexpr (Entered)
            {
                if (!start) // The first one to track the mouse will assign itslef to be a root.
                {
                    start = asker;
                    return true;
                }
                return start == asker;
            }
            else
            {
                if (start == asker)
                {
                    start = 0;
                    return true;
                }
                return faux;
            }
        }
        // mouse: Is the mouse seized/captured by asker?
        bool captured(id_t asker) const
        {
            return swift == asker;
        }
        // mouse: Is the mouse seized/captured?
        bool captured() const
        {
            return swift;
        }
        // mouse: Seize specified mouse control.
        bool capture(id_t asker)
        {
            if (!swift || swift == asker)
            {
                swift = asker;
                if (index != mouse::noactive) locks |= 1 << index;
                return true;
            }
            return faux;
        }
        // mouse: Release specified mouse control.
        void setfree(bool forced = true)
        {
            forced |= index == mouse::noactive;
            locks = forced ? 0
                           : locks & ~(1 << index);
            if (!locks) swift = {};
        }
    };

    // input: Keybd tracker.
    struct keybd
    {
        enum prot
        {
            vt,
            w32,
        };
        struct type
        {
            static constexpr byte keypress = 0;
            static constexpr byte keypaste = 1;
            static constexpr byte imeinput = 2;
            static constexpr byte imeanons = 3;
            static constexpr byte kblayout = 4;
        };

        si32 nullkey = key::Key2;

        id_t gear_id{};
        si32 ctlstat{};
        time timecod{};
        si32 keystat{};
        si32 virtcod{};
        si32 scancod{};
        si32 keycode{};
        bool extflag{};
        bool handled{};
        si64 touched{};
        text cluster{};
        text vkchord{};
        text scchord{};
        text chchord{};
        byte payload{}; // keybd: Payload type.

        auto doinput()
        {
            return keystat && key::map::data(keycode).edit;
        }
        auto generic()
        {
            return keycode & -2;
        }
        void update(syskeybd& k)
        {
            k.syncto(*this);
            fire_keybd();
        }

        virtual void fire_keybd() = 0;
    };

    // input: Focus tracker.
    struct focus
    {
        enum prot
        {
            w32,
            dec,
        };

        bool state{};
        si64 treeid{};
        ui64 digest{};

        void update(sysfocus& f)
        {
            state = f.state;
            treeid = f.treeid;
            digest = f.digest;
            fire_focus();
        }

        virtual void fire_focus() = 0;
    };

    // input: Clipboard tracker.
    struct board
    {
        enum prot
        {
            w32,
            dec,
        };

        using clip = clipdata;

        clip cargo{}; // board: Clipboard data.
        face image{}; // board: Clipboard preview render.
        bool shown{}; // board: Preview output tracker.
        si32 ghost{}; // board: Preview shadow size.
        cell brush{}; // board: Preview default color.
        byte alpha{}; // board: Preview transparency.

        virtual void fire_board() = 0;

        static void normalize(clipdata& c, id_t gear_id, time hash, twod winsz, qiew utf8, si32 form, qiew meta = {})
        {
            auto size = dot_00;
            if (form == mime::disabled && meta) // Try to parse meta: mime/size_x/size_y;data
            {
                     if (meta.starts_with(mime::tag::ansi)) { meta.remove_prefix(mime::tag::ansi.length()); form = mime::ansitext; }
                else if (meta.starts_with(mime::tag::text)) { meta.remove_prefix(mime::tag::text.length()); form = mime::textonly; }
                else if (meta.starts_with(mime::tag::rich)) { meta.remove_prefix(mime::tag::rich.length()); form = mime::richtext; }
                else if (meta.starts_with(mime::tag::html)) { meta.remove_prefix(mime::tag::html.length()); form = mime::htmltext; }
                else if (meta.starts_with(mime::tag::safe)) { meta.remove_prefix(mime::tag::safe.length()); form = mime::safetext; }

                if (form != mime::disabled)
                {
                    if (meta && meta.front() == '/') // Proceed preview size if present.
                    {
                        meta.remove_prefix(1);
                        if (auto v = utf::to_int(meta))
                        {
                            static constexpr auto max_value = twod{ 1000, 500 }; //todo unify
                            size.x = v.value();
                            if (meta)
                            {
                                meta.remove_prefix(1);
                                v = utf::to_int(meta);
                                if (v) size.y = v.value();
                            }
                            if (!size.x || !size.y) size = dot_00;
                            else                    size = std::clamp(size, dot_00, max_value);
                        }
                    }
                    meta = {};
                }
            }
            size = utf8.empty() ? dot_00
                 : size         ? size
                 : winsz        ? winsz
                                : twod{ 80,25 }; //todo make it configurable
            c.set(gear_id, hash, size, utf8.str(), form, meta.str());
        }
        static void normalize(clipdata& c)
        {
            normalize(c, id_t{}, c.hash, c.size, c.utf8, c.form, c.meta);
        }
        auto clear_clipboard()
        {
            auto not_empty = !!board::cargo.utf8.size();
            board::cargo.set(id_t{}, datetime::now(), dot_00, text{}, mime::ansitext, text{});
            fire_board();
            return not_empty;
        }
        void set_clipboard(clipdata const& data)
        {
            board::cargo.set(data);
            fire_board();
        }
        void set_clipboard(twod size, qiew utf8, si32 form)
        {
            auto c = clip{};
            c.set(id_t{}, datetime::now(), size, utf8.str(), form, text{});
            set_clipboard(c);
        }
        void update(sysboard& b) // Update clipboard preview.
        {
            board::image.move(dot_00); // Reset basis.
            auto draw_shadow = [](auto& canvas, auto& block, auto shadow_radius, twod trim_to)
            {
                canvas.mark(cell{});
                canvas.wipe();
                canvas.size(dot_21 * shadow_radius * 2 + trim_to);
                auto full = rect{ dot_21 * shadow_radius + dot_21, trim_to };
                auto temp = vrgb{};
                while (shadow_radius--)
                {
                    canvas.reset();
                    canvas.full(full);
                    canvas.template output<true>(block, cell::shaders::color(cell{}.bgc(0).fgc(0).alpha(0x60)));
                    canvas.template blur<true>(1, temp, [&](cell& c){ c.fgc(c.bgc()).txt(""); });
                }
                full.coor -= dot_21;
                canvas.reset();
                canvas.full(full);
            };
            if (b.form == mime::safetext)
            {
                auto blank = ansi::bgc(0x7Fffffff).fgc(0xFF000000).add(" Protected Data "); //todo unify (i18n)
                auto block = page{ blank };
                if (ghost) draw_shadow(board::image, block, ghost, block.current().size());
                else
                {
                    board::image.size(block.current().size());
                    board::image.wipe();
                }
                board::image.output(block);
            }
            else
            {
                auto block = page{ b.utf8 };
                if (ghost) draw_shadow(board::image, block, ghost, b.size);
                else
                {
                    board::image.size(b.size);
                    board::image.wipe();
                }
                board::image.mark(cell{});
                auto plain = b.form == mime::textonly || b.form == mime::disabled;
                if (plain) board::image.output<true>(block, cell::shaders::color(  brush));
                else       board::image.output<true>(block, cell::shaders::xlucent(alpha));
            }
        }
    };

    // input: Human interface device controller.
    struct hids
        : public mouse,
          public keybd,
          public focus,
          public board,
          public base
    {
        using list = std::list<wptr>;
        using kmap = std::unordered_map<si32, text>;

        enum modifiers
        {
            LCtrl        = 1 <<  0, // Left  ⌃ Ctrl
            RCtrl        = 1 <<  1, // Right ⌃ Ctrl
            LAlt         = 1 <<  2, // Left  ⎇ Alt, Left  ⌥ Option
            RAlt         = 1 <<  3, // Right ⎇ Alt, Right ⌥ Option, ⇮ AltGr
            LShift       = 1 <<  4, // Left  ⇧ Shift
            RShift       = 1 <<  5, // Right ⇧ Shift
            LWin         = 1 <<  6, // Left  ⊞ Win, ◆ Meta, ⌘ Cmd (Apple key), ❖ Super
            RWin         = 1 <<  7, // Right ⊞ Win, ◆ Meta, ⌘ Cmd (Apple key), ❖ Super
            NumLock      = 1 << 12, // ⇭ Num Lock
            CapsLock     = 1 << 13, // ⇪ Caps Lock
            ScrlLock     = 1 << 14, // ⇳ Scroll Lock (⤓)
            AltGr        = LAlt   | LCtrl,
            anyCtrl      = LCtrl  | RCtrl,
            anyAlt       = LAlt   | RAlt,
            anyShift     = LShift | RShift,
            anyAltGr     = anyAlt | anyCtrl,
            anyWin       = LWin   | RWin,
            anyMod       = anyAlt | anyCtrl | anyShift | anyWin,
        };

        static auto build_alone_key()
        {
            return std::unordered_map<si32, text>
            {
                { key::Backspace,     "\x7f"     },
                { key::Tab,           "\x09"     },
                { key::Pause,         "\x1a"     },
                { key::Esc,           "\033"     },
                { key::KeyPageUp,     "\033[5~"  },
                { key::KeyPageDown,   "\033[6~"  },
                { key::KeyEnd,        "\033[F"   },
                { key::KeyHome,       "\033[H"   },
                { key::KeyLeftArrow,  "\033[D"   },
                { key::KeyUpArrow,    "\033[A"   },
                { key::KeyRightArrow, "\033[C"   },
                { key::KeyDownArrow,  "\033[B"   },
                { key::KeyInsert,     "\033[2~"  },
                { key::KeyDelete,     "\033[3~"  },
                { key::F1,            "\033OP"   },
                { key::F2,            "\033OQ"   },
                { key::F3,            "\033OR"   },
                { key::F4,            "\033OS"   },
                { key::F5,            "\033[15~" },
                { key::F6,            "\033[17~" },
                { key::F7,            "\033[18~" },
                { key::F8,            "\033[19~" },
                { key::F9,            "\033[20~" },
                { key::F10,           "\033[21~" },
                { key::F11,           "\033[23~" },
                { key::F12,           "\033[24~" },
            };
        }
        static auto build_shift_key()
        {
            return std::unordered_map<si32, text>
            {
                { key::KeyPageUp,     "\033[5; ~"  },
                { key::KeyPageDown,   "\033[6; ~"  },
                { key::KeyEnd,        "\033[1; F"  },
                { key::KeyHome,       "\033[1; H"  },
                { key::KeyLeftArrow,  "\033[1; D"  },
                { key::KeyUpArrow,    "\033[1; A"  },
                { key::KeyRightArrow, "\033[1; C"  },
                { key::KeyDownArrow,  "\033[1; B"  },
                { key::KeyInsert,     "\033[2; ~"  },
                { key::KeyDelete,     "\033[3; ~"  },
                { key::F1,            "\033[1; P"  },
                { key::F2,            "\033[1; Q"  },
                { key::F3,            "\033[1; R"  },
                { key::F4,            "\033[1; S"  },
                { key::F5,            "\033[15; ~" },
                { key::F6,            "\033[17; ~" },
                { key::F7,            "\033[18; ~" },
                { key::F8,            "\033[19; ~" },
                { key::F9,            "\033[20; ~" },
                { key::F10,           "\033[21; ~" },
                { key::F11,           "\033[23; ~" },
                { key::F12,           "\033[24; ~" },
            };

        }
        static auto build_other_key(si32 slash, si32 quest)
        {
            return std::unordered_map<si32, text>
            {
                { key::KeyEnter  | hids::anyCtrl  << 8, { "\x0a"      }},
                { key::Backspace | hids::anyCtrl  << 8, { "\x08"      }},
                { key::Backspace | hids::anyAlt   << 8, { "\033\x7f"  }},
                { key::Backspace | hids::anyAltGr << 8, { "\033\x08"  }},
                { key::Tab       | hids::anyCtrl  << 8, { "\t"        }},
                { key::Tab       | hids::anyShift << 8, { "\033[Z"    }},
                { key::Tab       | hids::anyAlt   << 8, { "\033[1;3I" }},
                { key::Esc       | hids::anyAlt   << 8, { "\033\033"  }},
                { key::Key1      | hids::anyCtrl  << 8, { "1"         }},
                { key::Key3      | hids::anyCtrl  << 8, { "\x1b"      }},
                { key::Key4      | hids::anyCtrl  << 8, { "\x1c"      }},
                { key::Key5      | hids::anyCtrl  << 8, { "\x1d"      }},
                { key::Key6      | hids::anyCtrl  << 8, { "\x1e"      }},
                { key::Key7      | hids::anyCtrl  << 8, { "\x1f"      }},
                { key::Key8      | hids::anyCtrl  << 8, { "\x7f"      }},
                { key::Key9      | hids::anyCtrl  << 8, { "9"         }},
                { key::KeySlash  | hids::anyCtrl  << 8, { "\x1f"      }},
                { slash          | hids::anyAltGr << 8, { "\033\x1f"  }},
                { slash          | hids::anyCtrl  << 8, { "\x1f"      }},
                { quest          | hids::anyAltGr << 8, { "\033\x7f"  }},
                { quest          | hids::anyCtrl  << 8, { "\x7f"      }},
            };
        }

        id_t        relay; // hids: Mouse routing call stack initiator.
        base&       owner;
        core const& idmap; // hids: Area of the main form. Primary or relative region of the mouse coverage.
        bool        alive; // hids: Whether event processing is complete.

        //todo unify
        span tooltip_timeout; // hids: .
        text tooltip_data; // hids: Tooltip data.
        ui32 digest = 0; // hids: Tooltip digest.
        ui32 digest_tracker = 0; // hids: Tooltip changes tracker.
        ui32 tooltip_digest = 0; // hids: Tooltip digest.
        time tooltip_time = {}; // hids: The moment to show tooltip.
        bool tooltip_show = faux; // hids: Show tooltip or not.
        bool tooltip_stop = true; // hids: Disable tooltip.
        bool tooltip_set  = faux; // hids: Tooltip has been set.
        twod tooltip_coor = {}; // hids: .

        //todo unify
        rect slot; // slot for pro::maker and e2::createby.
        bool slot_forced = faux; // slot is preferred over cfg.winsize.

        //todo unify
        bool interrupt_key_proc{}; // hids: .
        netxs::sptr<text> script_ptr; // hids: A script body passed by pro::keybd/ui::menu.
        netxs::sptr<std::unordered_map<text, wptr>> scripting_context_ptr; // hids: Script execution context: sptr<map<$object_name_str, $object_wptr>>.
        qiew call_proc; // hids: .

        //todo unify
        bool mouse_disabled = faux; // Hide mouse cursor.
        bool keybd_disabled = faux; // Inactive gear.
        si32 countdown = 0;

        id_t user_index; // hids: User/Device image/icon index.
        kmap other_key; // hids: Dynamic key-vt mapping.

        bool shared_event = faux; // hids: The key event was touched by another procees/handler. See pro::keybd(release, key::post) for detailts.

        multihome_t& multihome; // hids: .

        hids(auth& indexer, base& owner, core const& idmap)
            : base{ indexer },
              relay{ 0 },
              owner{ owner },
              idmap{ idmap },
              alive{ faux },
              other_key{ build_other_key(key::KeySlash, key::KeySlash | (hids::anyShift << 8)) }, // Defaults for US layout.
              multihome{ owner.base::property<multihome_t>("multihome") }
        {
            mouse::prime = dot_mx;
            mouse::coord = dot_mx;
            keybd::gear_id = bell::id;
            base::signal(tier::general, input::events::device::user::login, user_index);
        }
        virtual ~hids()
        {
            mouse_leave(mouse::hover, mouse::start);
            release_if_captured();
            base::signal(tier::release, input::events::halt, *this);
            base::signal(tier::general, input::events::halt, *this);
            base::signal(tier::release, input::events::die, *this);
            base::signal(tier::general, input::events::die, *this);
            base::signal(tier::general, input::events::device::user::logout, user_index);
        }

        // hids: Whether event processing is complete.
        operator bool () const
        {
            return alive;
        }

        void set_multihome()
        {
            auto [world_wptr, parent_wptr] = multihome;
            if (auto world_ptr = world_wptr.lock())
            {
                world_ptr->base::father = parent_wptr;
            }
        }
        auto tooltip_enabled(time const& now)
        {
            return !mouse::m_sys.buttons
                && !mouse_disabled
                && !tooltip_stop
                && tooltip_show
                && tooltip_data.size()
                && tooltip_time < now
                && !captured();
        }
        void set_tooltip(view data, bool update = faux)
        {
            tooltip_set = true;
            if (!update || data != tooltip_data)
            {
                tooltip_data = data;
                if (update)
                {
                    if (tooltip_digest == digest) // To show tooltip even after clicks.
                    {
                        ++tooltip_digest;
                    }
                    if (!tooltip_stop) tooltip_stop = data.empty();
                }
                else
                {
                    tooltip_show = faux;
                    tooltip_stop = data.empty();
                    tooltip_time = datetime::now() + tooltip_timeout;
                }
                digest++;
            }
        }
        auto is_tooltip_changed()
        {
            return std::exchange(digest_tracker, digest) != digest_tracker;
        }
        auto get_tooltip()
        {
            return std::pair{ qiew{ tooltip_data }, tooltip_digest == digest };
        }
        void tooltip_recalc(hint deed)
        {
            if (deed == input::events::mouse::move.id)
            {
                if (tooltip_coor(mouse::coord) || (tooltip_show && tooltip_digest != digest)) // Do nothing on shuffle.
                {
                    if (tooltip_show && tooltip_digest == digest) // Drop tooltip if moved.
                    {
                        tooltip_stop = true;
                    }
                    else
                    {
                        tooltip_time = datetime::now() + tooltip_timeout;
                    }
                }
            }
            else if (deed == input::events::mouse::scroll::act.id) // Drop tooltip away.
            {
                tooltip_stop = true;
            }
            else
            {
                if (tooltip_show == faux) // Reset timer to begin,
                {
                    tooltip_time = datetime::now() + tooltip_timeout;
                }
            }
        }
        auto tooltip_check(time now) // Called every timer tick.
        {
            if (tooltip_stop) return faux;
            if (!tooltip_show && tooltip_time < now && !captured())
            {
                tooltip_show = true;
                if (tooltip_digest != digest)
                {
                    tooltip_digest = digest;
                    return true;
                }
            }
            else if (captured())
            {
                if (!tooltip_show) // If not shown then drop tooltip.
                {
                    tooltip_stop = true;
                }
                //else if (tooltip_time < now) // Give time to update tooltip text after mouse button release.
                //{
                //    tooltip_time = now + 100ms;
                //}
            }
            return faux;
        }

        void replay(sptr object_ptr, hint new_cause, fp2d new_coord, fp2d new_click, fp2d new_delta, si32 new_button_state, si32 new_ctlstate, fp32 new_whlfp, si32 new_whlsi, bool new_hzwhl)
        {
            static constexpr auto mask = netxs::events::level_mask(input::events::mouse::button::any.id);
            static constexpr auto base = mask & input::events::mouse::button::any.id;
            alive = true;
            keybd::ctlstat = new_ctlstate;
            mouse::coord = new_coord;
            mouse::click = new_click;
            mouse::whlfp = new_whlfp;
            mouse::whlsi = new_whlsi;
            mouse::hzwhl = new_hzwhl;
            mouse::cause = (new_cause & ~mask) | base; // Remove the dependency on the event tree root.
            mouse::delta.set(new_delta);
            mouse::load_button_state(new_button_state);
            pass(tier::release, object_ptr, owner.base::coor(), true);
        }

        auto meta(si32 ctl_key = -1) { return keybd::ctlstat & ctl_key; }

        // hids: Stop handeling this event.
        void dismiss(bool set_nodbl = faux)
        {
            alive = faux;
            if (set_nodbl) nodbl = true;
        }
        void dismiss_dblclick()
        {
            nodbl = true;
        }
        void set_handled()
        {
            keybd::handled = true;
        }

        void take(sysfocus& f)
        {
            if (f.state) keybd_disabled = faux;
            focus::update(f);
        }
        void take(sysmouse& m)
        {
            #if defined(DEBUG)
            if (m.wheelsi)
            {
                auto s = m.ctlstat;
                auto alt     = s & hids::anyAlt ? 1 : 0;
                auto l_ctrl  = s & hids::LCtrl  ? 1 : 0;
                auto r_ctrl  = s & hids::RCtrl  ? 1 : 0;
                     if (l_ctrl && alt) netxs::_k2 += m.wheelsi > 0 ? 1 : -1; // LCtrl + Alt t +Wheel.
                else if (l_ctrl)        netxs::_k0 += m.wheelsi > 0 ? 1 : -1; // LCtrl+Wheel.
                else if (alt)           netxs::_k1 += m.wheelsi > 0 ? 1 : -1; // Alt+Wheel.
                else if (r_ctrl)        netxs::_k3 += m.wheelsi > 0 ? 1 : -1; // RCtrl+Wheel.
            }
            #endif
            mouse_disabled = faux;
            keybd_disabled = faux;
            keybd::ctlstat = m.ctlstat;
            mouse::update(m, idmap);
        }
        void take(syskeybd& k)
        {
            keybd_disabled = faux;
            if (k.keycode == key::config) // Receive three layout related values coded as codepoints: nullkey keycode, '/' keycode+mods, '?' keycode+mods.
            {
                auto i = utf::cpit{ k.cluster };
                keybd::nullkey = i.next().cdpoint;
                auto slash     = i.next().cdpoint;
                auto quest     = i.next().cdpoint;
                other_key = build_other_key(slash, quest);
            }
            else
            {
                tooltip_stop = true;
                keybd::update(k);
            }
        }
        auto take(sysboard& b)
        {
            board::update(b);
        }

        auto& area() const { return idmap.area(); }

        void pass(si32 Tier, sptr object_ptr, fp2d offset, bool relative = faux)
        {
            if (object_ptr)
            {
                auto temp_coord = mouse::coord;
                auto temp_click = mouse::click;
                if (relative)
                {
                    object_ptr->global(coord);
                    click += coord - temp_coord;
                }
                mouse::coord += offset;
                mouse::click += offset;
                object_ptr->base::signal(Tier, mouse::cause, *this);
                mouse::coord = temp_coord;
                mouse::click = temp_click;
            }
        }
        void mouse_leave(id_t last_id, id_t start_id)
        {
            if (last_id)
            {
                if (auto last = bell::getref<base>(last_id))
                {
                    auto saved_start = std::exchange(mouse::start, start_id);
                    auto saved_cause = std::exchange(mouse::cause, input::events::mouse::hover::leave.id);
                    last->base::signal(tier::release, mouse::cause, *this);
                    mouse::start = saved_start;
                    mouse::cause = saved_cause;
                }
                else
                {
                    //todo revise
                    log("%%Error condition: Clients count is broken, dangling %last_id%", prompt::hids, last_id);
                }
            }
        }
        void redirect_mouse_focus(base& boss)
        {
            if (mouse::hover != boss.id) // The mouse cursor is over the new object.
            {
                if (tooltip_data.size())
                {
                    digest++;
                    tooltip_data.clear();
                    tooltip_stop = true;
                }
                tooltip_set = faux;
                // Firing the leave event right after the enter allows us
                // to avoid flickering the parent object state when focus
                // acquired by children.
                auto start_leave = std::exchange(mouse::start, 0); // The first one to track the mouse will assign itself by calling gear.direct<true>(id).
                auto saved_cause = std::exchange(mouse::cause, input::events::mouse::hover::enter.id);
                boss.base::signal(tier::release, mouse::cause, *this);
                mouse_leave(mouse::hover, start_leave);
                mouse::hover = boss.id;
                mouse::cause = saved_cause;
            }
        }
        void release_if_captured()
        {
            if (captured())
            {
                setfree();
                dismiss();
            }
        }
        void deactivate()
        {
            mouse::load_button_state(0);
            mouse::m_sys.buttons = {};
            redirect_mouse_focus(owner);
            release_if_captured();
            base::signal(tier::general, input::events::halt, *this);
            mouse_disabled = true;
            keybd_disabled = true;
        }
        void okay(base& boss)
        {
            if (boss.id == relay)
            {
                redirect_mouse_focus(boss);
                boss.base::signal(tier::release, mouse::cause, *this);
            }
        }
        void fire(hint new_cause, si32 new_index = mouse::noactive)
        {
            if (mouse_disabled) return;

            alive = true;
            mouse::index = new_index;
            mouse::cause = new_cause;
            mouse::coord = mouse::prime;
            mouse::nodbl = faux;

            auto offset = idmap.coor();
            if (mouse::swift)
            {
                auto next = bell::getref<base>(mouse::swift);
                if (next)
                {
                    redirect_mouse_focus(*next);
                    pass(tier::release, next, offset, true);

                    if (alive && !captured()) // Pass unhandled event to the gate.
                    {
                        owner.base::signal(tier::release, new_cause, *this);
                    }
                }
                else mouse::setfree();
            }
            else
            {
                if (!tooltip_stop) tooltip_recalc(new_cause);
                owner.base::signal(tier::preview, new_cause, *this);

                if (!alive) return;

                auto next = idmap.link(mouse::coord);
                if (next != owner.id)
                {
                    relay = next;
                    pass(tier::preview, bell::getref<base>(next), offset, true);
                    relay = 0;

                    if (!alive) return;
                }

                owner.base::signal(tier::release, new_cause, *this); // Pass unhandled event to the gate.
            }
        }
        bool fire_fast()
        {
            if (mouse_disabled) return true;
            alive = true;
            auto next_id = mouse::swift ? mouse::swift
                                        : idmap.link(m_sys.coordxy);
            if (next_id != owner.id)
            {
                if (auto next_ptr = bell::getref<base>(next_id))
                {
                    auto& next = *next_ptr;
                    auto  temp = m_sys.coordxy;
                    m_sys.coordxy += idmap.coor();
                    next.global(m_sys.coordxy);
                    next.base::signal(tier::release, input::events::device::mouse::on, *this);
                    m_sys.coordxy = temp;
                    if (!alive) // Clear one-shot events on success.
                    {
                        m_sys.wheelfp = {};
                        m_sys.wheelsi = {};
                        m_sys.hzwheel = {};
                    }
                }
                else if (mouse::swift == next_id) mouse::setfree();
            }
            if (m_sav.changed != m_sys.changed) m_sav = m_sys;
            return !alive;
        }
        void fire_keybd()
        {
            owner.base::signal(tier::preview, input::events::keybd::key::post, *this);
        }
        void fire_board()
        {
            owner.base::signal(tier::release, input::events::clipboard, *this);
            mouse::delta.set(); // Update time stamp.
        }
        void fire_focus()
        {
            focus::state ? owner.base::signal(tier::release, input::events::focus::set::on,  { .gear_id = id, .just_activate_only = true, .treeid = focus::treeid, .digest = focus::digest })
                         : owner.base::signal(tier::release, input::events::focus::set::off, { .gear_id = id,                             .treeid = focus::treeid, .digest = focus::digest });
        }
        text interpret(bool decckm)
        {
            static auto alone_key = build_alone_key();
            static auto shift_key = build_shift_key();

            if (keystat != input::key::released)
            {
                auto s = keybd::ctlstat;
                auto v = keybd::keycode & -2; // Generic keys only
                auto c = keybd::cluster.empty() ? 0 : keybd::cluster.front();

                if (s & hids::LCtrl && s & hids::RAlt) // This combination is already translated.
                {
                    s &= ~(hids::LCtrl | hids::RAlt);
                }

                auto shift = s & hids::anyShift ? hids::anyShift : 0;
                auto alt   = s & hids::anyAlt   ? hids::anyAlt   : 0;
                auto ctrl  = s & hids::anyCtrl  ? hids::anyCtrl  : 0;
                if (shift || alt || ctrl)
                {
                    if (ctrl && alt) // c == 0 for ctrl+alt+key combinationsons on windows.
                    {
                        if (c == 0) // Chars and vkeys for ' '(0x20),'A'-'Z'(0x41-5a) are the same on windows.
                        {
                                 if (v >= key::KeyA  && v <= key::KeyZ)      return "\033"s + (char)((0x41 + (v - key::KeyA) / 2) & 0b00011111);//generate('\033', (wchr)( a  & 0b00011111)); // Alt causes to prepend '\033'. Ctrl trims by 0b00011111.
                            else if (v == key::Space || v == keybd::nullkey) return "\033\0"s;  //'\033' + (wchr)('@' & 0b00011111)); // Map ctrl+alt+@ to ^[^@;
                        }
                        else if (c == 0x20 || (c >= 'A' && c <= 'Z')) return "\033"s + (char)(c & 0b00011111);//generate('\033', (wchr)( a  & 0b00011111)); // Alt causes to prepend '\033'. Ctrl trims by 0b00011111.
                    }

                    if (auto it_shift = shift_key.find(v); it_shift != shift_key.end())
                    {
                        auto& mods = *++(it_shift->second.rbegin());
                        mods = '1';
                        if (shift) mods += 1;
                        if (alt  ) mods += 2;
                        if (ctrl ) mods += 4;
                        return it_shift->second;
                    }
                    else if (auto it_other = other_key.find(v | (shift | alt | ctrl) << 8); it_other != other_key.end())
                    {
                        return it_other->second;
                    }
                    else if (!ctrl &&  alt && c) return text{ '\033' + keybd::cluster };
                    else if ( ctrl && !alt)
                    {
                             if (c == 0x20 || (c == 0x00 && v == keybd::nullkey)) return text(1, '@' & 0b00011111); // Detect ctrl+@ and ctrl+space.
                        else if (c == 0x00 && (v >= key::KeyA && v <= key::KeyZ)) return text(1, (0x41 + (v - key::KeyA) / 2) & 0b00011111); // Emulate ctrl+key mapping to C0 if current kb layout does not contain it.
                    }
                }

                if (auto it_alone = alone_key.find(v); it_alone != alone_key.end())
                {
                    if (v >= key::KeyEnd && v <= key::KeyDownArrow) it_alone->second[1] = decckm ? 'O' : '[';
                    return it_alone->second;
                }
                else if (c) return keybd::cluster;
            }
            return text{};
        }
    };
}
// Copyright (c) Dmitry Sapozhnikov
// Licensed under the MIT license.

#include "ansivt.hpp"

#pragma once

namespace netxs::prompt
{
    static constexpr auto  pads = "      "sv;
    static constexpr auto    os = "  os: "sv;
    static constexpr auto   gui = " gui: "sv;
    static constexpr auto   ack = " ack: "sv;
    static constexpr auto   key = " key: "sv;
    static constexpr auto   lua = " lua: "sv;
    static constexpr auto   tty = " tty: "sv;
    static constexpr auto   vtm = " vtm: "sv;
    static constexpr auto   xml = " xml: "sv;
    static constexpr auto   cin = "stdin: "sv;
    static constexpr auto  cout = "stdout: "sv;
    static constexpr auto resultant_settings = "Resultant settings:"sv;

    #define prompt_list \
        X(apps) /* */ \
        X(args) /* */ \
        X(auth) /* */ \
        X(base) /* */ \
        X(calc) /* */ \
        X(desk) /* */ \
        X(diff) /* */ \
        X(dtvt) /* */ \
        X(exec) /* */ \
        X(foci) /* */ \
        X(gate) /* */ \
        X(hall) /* */ \
        X(hids) /* */ \
        X(host) /* */ \
        X(logs) /* */ \
        X(main) /* */ \
        X(meet) /* */ \
        X(nt32) /* */ \
        X(nt64) /* */ \
        X(open) /* */ \
        X(page) /* */ \
        X(para) /* */ \
        X(path) /* */ \
        X(pipe) /* */ \
        X(pool) /* */ \
        X(rail) /* */ \
        X(repl) /* */ \
        X(s11n) /* */ \
        X(sock) /* */ \
        X(term) /* */ \
        X(task) /* */ \
        X(text) /* */ \
        X(tile) /* */ \
        X(user) /* */ \
        X(vtty) /* */ \
        X(wait) /* */ \
        X(xipc) /* */ \
        X(xtty) /* */

    #define X(prompt) static constexpr auto prompt = #prompt": "sv;
        prompt_list
    #undef X
    #undef prompt_list
}

namespace netxs::directvt
{
    using namespace netxs::ansi;

    namespace binary
    {
        using type = byte;

        static constexpr auto is_list = type{ 1 << (sizeof(type) * 8 - 1) };

        struct blob : public view
        { };

        #pragma pack(push,1)
        struct marker
        {
            static constexpr auto initial = (byte)'\xFF';

            using sz_t = le_t<netxs::sz_t>;
            using type = le_t<netxs::twod::type>;

            byte mark_FF;
            sz_t cfgsize;
            type winx_sz;
            type winy_sz;
            byte mark_FE;

            marker()
            { }
            marker(size_t config_size, twod winsz)
            {
                mark_FF = initial;
                cfgsize.set((netxs::sz_t)config_size);
                winx_sz.set(winsz.x);
                winy_sz.set(winsz.y);
                mark_FE = initial - 1;
            }

            auto get(netxs::sz_t& config_size, twod& winsz)
            {
                if (mark_FF == initial
                 && mark_FE == initial - 1)
                {
                    config_size = cfgsize.get();
                    winsz = twod{ winx_sz.get(), winy_sz.get() };
                    return true;
                }
                else return faux;
            }
            auto data() { return reinterpret_cast<char*>(this); }
            auto size() { return sizeof(*this); }
        };
        #pragma pack(pop)

        template<class T>
        void fuse_ext(auto& block, T&& data)
        {
            using D = std::remove_cv_t<std::remove_reference_t<T>>;
            if constexpr (std::is_same_v<D, char>
                       || std::is_same_v<D, int8>
                       || std::is_same_v<D, byte>)
            {
                block.text::push_back((char)data);
            }
            else if constexpr (std::is_arithmetic_v<D>
                            || std::is_same_v<D, twod>
                            || std::is_same_v<D, fp2d>
                            || std::is_same_v<D, dent>
                            || std::is_same_v<D, rect>)
            {
                auto le_data = netxs::letoh(data);
                block += view{ (char*)&le_data, sizeof(le_data) };
            }
            else if constexpr (std::is_same_v<D, argb>)
            {
                block += view{ (char*)&data, sizeof(data) };
            }
            else if constexpr (std::is_same_v<D, view>
                            || std::is_same_v<D, qiew>
                            || std::is_same_v<D, text>)
            {
                auto length = (sz_t)data.length();
                auto le_len = netxs::letoh(length);
                block += view{ (char*)&le_len, sizeof(le_len) };
                block += data;
            }
            else if constexpr (std::is_same_v<D, time>)
            {
                auto n = data.time_since_epoch().count();
                auto le_n = netxs::letoh(n);
                block += view{ (char*)&le_n, sizeof(le_n) };
            }
            else if constexpr (std::is_same_v<D, std::unordered_map<text, text>>
                            || std::is_same_v<D, std::map<text, text>>
                            || std::is_same_v<D, generics::imap<text, text>>)
            {
                //todo implement
            }
            else if constexpr (std::is_same_v<D, noop>)
            {
                // Noop.
            }
            else if constexpr (std::is_same_v<D, std::any>)
            {
                // Four letters type encoding.
                #define type_id_list \
                    X(bool) \
                    X(text) \
                    X(char) \
                    X(int8) \
                    X(byte) \
                    X(ui16) \
                    X(ui32) \
                    X(ui64) \
                    X(si16) \
                    X(si32) \
                    X(si64) \
                    X(twod) \
                    X(rect) \
                    X(fp32) \
                    X(fp64) \
                    X(fp2d) \
                    X(time) \
                    X(span) \
                    X(argb) \
                    X(dent)
                #define X(item_t)   if (data.type() == typeid(item_t)) {                \
                                        fuse_ext(block, make_ui32(#item_t));            \
                                        fuse_ext(block, *std::any_cast<item_t>(&data)); \
                                    } else
                type_id_list
                {
                    log(prompt::dtvt, "Unsupported data type");
                }
                #undef X
            }
            else if constexpr (requires{ std::begin(std::declval<D>()); })
            {
                auto length = (sz_t)data.size();
                auto le_len = netxs::letoh(length);
                block += view{ (char*)&le_len, sizeof(le_len) };
                for (auto& item : data) fuse_ext(block, item);
            }
            else log(prompt::dtvt, "Unsupported data type");
        }

        struct packet : text
        {
            void fuse(auto&& data)            { fuse_ext(*this, std::forward<decltype(data)>(data)); }
            auto& add(auto&& data)            { fuse(std::forward<decltype(data)>(data)); return *this; }
            auto& add(auto*  data, auto size) { operator+=(view{ data, size }); return *this; }
            auto& add(auto&& data, auto&&... data_list)
            {
                fuse(std::forward<decltype(data)>(data));
                return add(std::forward<decltype(data_list)>(data_list)...);
            }
        };

        struct stream
        {
            type next{};

            constexpr explicit operator bool () const
            {
                return valid;
            }

        protected:
            escx block;
            sz_t basis;
            sz_t start;
            bool valid;

            // stream: .
            template<class T>
            void fuse(T&& data) { fuse_ext(block, std::forward<T>(data)); }
            // stream: Replace bytes at specified position.
            template<class T>
            inline auto& add_at(sz_t at, T&& data)
            {
                auto le_data = netxs::letoh(std::forward<T>(data));
                ::memcpy(block.text::data() + at, reinterpret_cast<void const*>(&le_data), sizeof(le_data));
                return *this;
            }
            // stream: .
            template<class T, bool PeekOnly = faux, class D = std::remove_cv_t<std::remove_reference_t<T>>, class R0 = std::conditional_t<std::is_same_v<D, noop>, si32, D>, class R = std::conditional_t<std::is_same_v<R0, text>, view, R0>>
            static R _take_item(view& data)
            {
                if constexpr (std::is_same_v<D, view> || std::is_same_v<D, text>)
                {
                    if (data.size() < sizeof(sz_t))
                    {
                        log(prompt::dtvt, "Corrupted frame header");
                        if constexpr (!PeekOnly) data.remove_prefix(data.size());
                        return view{};
                    }
                    auto size = netxs::aligned<sz_t>(data.data());
                    if (data.size() < size + sizeof(size))
                    {
                        log(prompt::dtvt, "Corrupted frame data");
                        if constexpr (!PeekOnly) data.remove_prefix(data.size());
                        return view{};
                    }
                    auto crop = view{ data.data() + sizeof(sz_t), (size_t)size };
                    if constexpr (!PeekOnly)
                    {
                        data.remove_prefix(sizeof(sz_t) + size);
                    }
                    return crop;
                }
                else if constexpr (std::is_same_v<D, blob>)
                {
                    auto crop = blob{ data };
                    if constexpr (!PeekOnly)
                    {
                        data.remove_prefix(data.size());
                    }
                    return crop;
                }
                else if constexpr (std::is_same_v<D, noop>)
                {
                    return 0;
                }
                else if constexpr (std::is_same_v<D, time>)
                {
                    using span = decltype(time{}.time_since_epoch());
                    using data_type = decltype(span{}.count());
                    if (data.size() < sizeof(data_type))
                    {
                        log(prompt::dtvt, "Corrupted datetime data");
                        if constexpr (!PeekOnly) data.remove_prefix(data.size());
                        return D{};
                    }
                    auto temp = netxs::aligned<data_type>(data.data());
                    auto crop = time{ span{ temp }};
                    if constexpr (!PeekOnly)
                    {
                        data.remove_prefix(sizeof(data_type));
                    }
                    return crop;
                }
                else if constexpr (std::is_same_v<D, std::any>)
                {
                    auto crop = std::any{};
                    if (data.size() <= sizeof(si32))
                    {
                        log(prompt::dtvt, "Corrupted frame data");
                        if constexpr (!PeekOnly) data.remove_prefix(data.size());
                        return crop;
                    }
                    auto type_id = netxs::aligned<si32>(data.data()); // Get four letters type index.
                    auto bits = data.substr(sizeof(si32));
                    switch (type_id)
                    {
                        #define X(item_t) case make_ui32(#item_t): \
                                            crop = _take_item<item_t>(bits); \
                                            break;
                        type_id_list
                        #undef X
                        #undef type_id_list
                    }
                    if constexpr (!PeekOnly) data = bits;
                    return crop;
                }
                else if constexpr (requires{ std::begin(std::declval<D>()); })
                {
                    using data_type = decltype(*std::begin(std::declval<D>()));
                    auto crop = D{};
                    if (data.size() < sizeof(sz_t))
                    {
                        log(prompt::dtvt, "Corrupted frame header");
                        if constexpr (!PeekOnly) data.remove_prefix(data.size());
                        return crop;
                    }
                    auto count = netxs::aligned<sz_t>(data.data());
                    if (data.size() < sizeof(count) + count) // At least 1 byte per element required.
                    {
                        log(prompt::dtvt, "Corrupted frame data");
                        if constexpr (!PeekOnly) data.remove_prefix(data.size());
                        return crop;
                    }
                    auto bits = data.substr(sizeof(sz_t));
                    while (count--) crop.push_back(_take_item<data_type>(bits));
                    if constexpr (!PeekOnly) data = bits;
                    return crop;
                }
                else
                {
                    if (data.size() < sizeof(D))
                    {
                        log(prompt::dtvt, "Corrupted integer data");
                        if constexpr (!PeekOnly) data.remove_prefix(data.size());
                        return D{};
                    }
                    auto crop = netxs::aligned<D>(data.data());
                    if constexpr (!PeekOnly)
                    {
                        data.remove_prefix(sizeof(D));
                    }
                    return crop;
                }
            }
            // stream: .
            template<class ...Fields>
            static auto _take(std::tuple<Fields...>, view& data)
            {
                return std::tuple{ _take_item<Fields>(data)..., };
            }

        public:
            // stream: .
            template<class T>
            inline auto& add(T&& data)
            {
                fuse(std::forward<T>(data));
                return *this;
            }
            // stream: .
            template<class Char, class Size_t>
            inline auto& add(Char* data, Size_t size)
            {
                block += view{ data, size };
                return *this;
            }
            // stream: .
            template<class T, class ...Args>
            inline auto& add(T&& data, Args&&... data_list)
            {
                fuse(std::forward<T>(data));
                return add(std::forward<Args>(data_list)...);
            }
            // stream: .
            template<class T>
            static void peek(T&& dest, view& data)
            {
                dest = _take_item<T, true>(data);
            }
            // stream: .
            template<class ...Fields>
            static auto take(view& data)
            {
                return std::tuple{ _take_item<Fields>(data)..., };
            }
            // stream: .
            template<class T>
            static void take(T&& dest, view& data)
            {
                dest = _take_item<T>(data);
            }
            // stream: .
            static void take(void *dest, size_t size, view& data)
            {
                ::memcpy(dest, reinterpret_cast<void const*>(data.data()), size);
                data.remove_prefix(size);
            }
            // stream: .
            static auto take_substr(size_t size, view& data)
            {
                if (size > data.size())
                {
                    log(prompt::dtvt, "Wrong data size");
                    return view{};
                }
                auto crop = data.substr(0, size);
                data.remove_prefix(size);
                return crop;
            }
            // stream: Check DirectVT frame integrity.
            static auto purify(view flow)
            {
                auto size = flow.size();
                auto head = flow.data();
                auto iter = head;
                while (size >= sizeof(sz_t))
                {
                    auto step = netxs::aligned<sz_t>(iter);
                    if (step < sizeof(sz_t))
                    {
                        log(prompt::dtvt, "Stream corrupted, frame size: ", step);
                        break;
                    }
                    if (size < step) break;
                    size -= step;
                    iter += step;
                }
                auto crop = qiew(head, iter - head);
                return crop;
            }
            // stream: .
            template<class T, class P, bool Plain = std::is_same_v<void, std::invoke_result_t<P, qiew>>>
            static void reading_loop(T& link, P&& proc)
            {
                auto flow = text{};
                while (link)
                {
                    auto shot = link.recv();
                    if (shot && link)
                    {
                        flow += shot;
                        if (auto crop = purify(flow))
                        {
                            if constexpr (Plain) proc(crop);
                            else            if (!proc(crop)) break;
                            flow.erase(0, crop.size()); // Delete processed data.
                        }
                    }
                    else break;
                }
            }
            // stream: .
            template<class T, class P>
            static void reading_loop(netxs::sptr<T> link_ptr, P&& proc)
            {
                reading_loop(*link_ptr, proc);
            }
            // stream: .
            auto length() const
            {
                return (sz_t)block.length();
            }
            // stream: .
            auto reset()
            {
                block.resize(basis);
                return sz_t{ 0 };
            }
            // stream: .
            template<class ...Args>
            void reinit(Args&&... args)
            {
                reset();
                add(std::forward<Args>(args)...);
                start = length();
            }
            // stream: .
            auto commit(bool discard_empty = faux)
            {
                auto size = length();
                if (discard_empty && size == start)
                {
                    return reset();
                }
                else
                {
                    stream::add_at(0, size);
                    return size;
                }
            }
            // stream: .
            template<bool Discard_empty = faux, bool Move = true, class T>
            void sendby(T&& sender)
            {
                if (stream::commit(Discard_empty))
                {
                    sender.output(block);
                    if constexpr (Move) stream::reset();
                }
            }
            // stream: .
            template<bool Discard_empty = faux, bool Move = true, class T>
            void sendby(netxs::sptr<T> sender_ptr)
            {
                sendby<Discard_empty, Move>(*sender_ptr);
            }
            template<bool Discard_empty = faux, class P>
            void sendfx(P output)
            {
                if (stream::commit(Discard_empty))
                {
                    output(block);
                    stream::reset();
                }
            }
            template<class T, class P>
            static auto read_block(T& object, P input)
            {
                auto buff = text(sizeof(sz_t), '\0');
                auto shot = input(buff.data(), buff.size());
                if (shot.size() != buff.size())
                {
                    if constexpr (debugmode) log(prompt::dtvt, "Stream interrupted");
                    return faux;
                }
                auto rest = netxs::aligned<sz_t>(buff.data());
                if (rest < sizeof(sz_t) || rest < shot.size())
                {
                    log(prompt::dtvt, "Stream corrupted, frame size: ", rest);
                    return faux;
                }
                rest -= (sz_t)shot.size();
                buff.resize(rest);
                auto head = buff.data();
                while (rest)
                {
                    shot = input(head, rest);
                    if (!shot)
                    {
                        log(prompt::dtvt, "Stream corrupted");
                        return faux;
                    }
                    auto s = (sz_t)shot.size();
                    rest -= s;
                    head += s;
                }
                auto data = view{ buff };
                auto kind = netxs::aligned<type>(data.data());
                if (kind != object.kind)
                {
                    log(prompt::dtvt, "Object type mismatch");
                    return faux;
                }
                data.remove_prefix(sizeof(type));
                object.get(data);
                return true;
            }
            // stream: .
            template<type Kind, class ...Args>
            void set(Args&&... args)
            {
                valid = true;
                add(Kind, std::forward<Args>(args)...);
            }
            // stream: .
            void emplace(stream& other)
            {
                other.commit();
                block += other.block;
                other.reset();
            }

            stream(stream const&) = default;
            stream(type kind)
                : basis{ sizeof(basis) + sizeof(kind) },
                  start{ basis                        },
                  valid{ true                         }
            {
                add(basis, kind);
            }
            stream& operator = (stream const&) = default;
        };

        template<class Base>
        class wrapper
        {
            friend struct access;
            using utex = std::recursive_mutex;
            using cond = std::condition_variable_any;
            using Lock = std::unique_lock<utex>;

            utex mutex; // wrapper: Accesss mutex.
            cond synch; // wrapper: Accesss notificator.
            Base thing; // wrapper: Protected object.
            flag alive{ true }; // wrapper: Connection status.

        public:
            static constexpr auto kind = Base::kind;

            struct access
            {
                Lock  guard; // access: .
                Base& thing; // access: .
                cond& synch; // access: .
                flag& alive; // access: .

                access(wrapper& w)
                    : guard{ w.mutex },
                      thing{ w.thing },
                      synch{ w.synch },
                      alive{ w.alive }
                { }
                access(access&& a)
                    : guard{ std::move(a.guard) },
                      thing{ a.thing },
                      synch{ a.synch },
                      alive{ a.alive }
                { }
               ~access()
                {
                    if (guard) synch.notify_all();
                }

                void unlock()
                {
                    if (guard)
                    {
                        synch.notify_all();
                        guard.unlock();
                    }
                }
                template<class Period>
                auto wait_for(Period&& maxoff)
                {
                    return alive ? synch.wait_for(guard, std::forward<Period>(maxoff))
                                 : std::cv_status::timeout;
                }
                void wait()
                {
                    if (alive) synch.wait(guard);
                }
            };

            // wrapper .
            auto freeze()
            {
                return access{ *this };
            }
            // wrapper .
            template<bool Discard_empty = faux, class T, class ...Args>
            void send(T&& sender, Args&&... args)
            {
                auto lock = freeze();
                if constexpr (sizeof...(args))
                {
                    thing.set(std::forward<Args>(args)...);
                }
                thing.template sendby<Discard_empty>(sender);
            }
            // wrapper .
            template<class ...Args>
            void set(Args&&... args)
            {
                auto lock = freeze();
                if constexpr (sizeof...(args))
                {
                    thing.set(std::forward<Args>(args)...);
                }
            }
            // wrapper .
            template<bool Discard_empty = faux, class T, class ...Args>
            void send(netxs::sptr<T> sender_ptr, Args&&... args)
            {
                send<Discard_empty>(*sender_ptr, std::forward<Args>(args)...);
            }
            // wrapper .
            template<class T>
            auto recv(netxs::sptr<T> link)
            {
                auto lock = freeze();
                thing.load([&](auto... args){ return link->recv(args...); });
                return thing;
            }
            // wrapper .
            template<class ...Args>
            auto operator () (Args&&... args)
            {
                auto lock = freeze();
                thing.set(std::forward<Args>(args)...);
                return lock;
            }
            // wrapper .
            auto sync(view& data)
            {
                auto lock = freeze();
                thing.get(data);
                return lock;
            }
            // wrapper .
            auto drop()
            {
                auto lock = freeze();
                alive = faux;
            }
        };

        template<class Base, class Type>
        struct list
            : stream
        {
            static constexpr auto kind = type{ Type::kind | binary::is_list };

            Base copy;
            Type item;

            struct iter
            {
                view  rest;
                view  crop;
                bool  stop;
                Type& item;

                iter(view data, Type& item)
                    : rest{ data },
                      stop{ faux },
                      item{ item }
                {
                    operator++();
                }
                template<class A>
                bool  operator == (A&&) const { return stop;                 }
                auto& operator  * ()    const { item.get(crop); return item; }
                auto& operator  * ()          { item.get(crop); return item; }
                auto  operator ++ ()
                {
                    static constexpr auto head = sizeof(sz_t) + sizeof(type);
                    stop = rest.size() < head;
                    if (stop) crop = {};
                    else
                    {
                        auto size = sz_t{};
                        std::tie(size, item.next) = stream::template take<sz_t, type>(rest);
                        stop = size > rest.size() + head;
                        if (stop) log(prompt::dtvt, "Corrupted data");
                        else
                        {
                            crop = rest.substr(0, size - head);
                            rest.remove_prefix(size - head);
                        }
                    }
                }
            };

        public:
            list(list const&) = default;
            list(list&&)      = default;
            list()
                : stream{ kind }
            { }

            auto begin() { return iter{ copy, item }; }
            auto   end() { return text::npos; }
            // list: .
            template<class ...Args>
            void push(Args&&... args)
            {
                item.set(std::forward<Args>(args)...);
                stream::emplace(item);
            }
            // list: .
            template<class ...Args>
            void set(Args&&... args)
            {
                item.set(std::forward<Args>(args)...);
            }
            // list: .
            void get(view& data)
            {
                copy = data;
                data = {};
            }
        };

        static constexpr auto _counter_base = __COUNTER__;

        // Definition of plain objects.
        #define DEFINE_macro
        #include "macrogen.hpp"

        #define STRUCT_macro(struct_name, struct_members)                                   \
            struct CAT_macro(struct_name, _t) : public stream                               \
            {                                                                               \
                static constexpr auto kind = type{ __COUNTER__ - _counter_base };           \
                SEQ_ATTR_macro(WRAP_macro(struct_members))                                  \
                CAT_macro(struct_name, _t)()                                                \
                    : stream{ kind }                                                        \
                { }                                                                         \
                void set()                                                                  \
                {                                                                           \
                    stream::reset();                                                        \
                    stream::add(SEQ_NAME_macro(WRAP_macro(struct_members)) noop{});         \
                }                                                                           \
                void set(SEQ_SIGN_macro(WRAP_macro(struct_members)) int /*_tmp*/ = {})      \
                {                                                                           \
                    SEQ_INIT_macro(WRAP_macro(struct_members))                              \
                    set();                                                                  \
                }                                                                           \
                void set(auto&& src)                                                        \
                {                                                                           \
                    SEQ_TEMP_macro(WRAP_macro(struct_members))                              \
                    set();                                                                  \
                }                                                                           \
                void syncto(auto& dst) const                                                \
                {                                                                           \
                    SEQ_SYNC_macro(WRAP_macro(struct_members))                              \
                }                                                                           \
                void get(view& _data)                                                       \
                {                                                                           \
                    int _tmp;                                                               \
                    std::tie(SEQ_NAME_macro(WRAP_macro(struct_members)) _tmp) =             \
                      stream::take<SEQ_TYPE_macro(WRAP_macro(struct_members)) noop>(_data); \
                }                                                                           \
                auto load(auto _recv)                                                       \
                {                                                                           \
                    stream::valid = stream::read_block(*this, _recv);                       \
                    return stream::valid;                                                   \
                }                                                                           \
                auto load(qiew _data)                                                       \
                {                                                                           \
                    stream::valid = stream::read_block(*this, [&](auto dst, auto len)       \
                    {                                                                       \
                        if (_data.size() >= len)                                            \
                        {                                                                   \
                            std::copy(_data.begin(), _data.begin() + len, dst);             \
                            _data.remove_prefix(len);                                       \
                            return qiew{ dst, len };                                        \
                        }                                                                   \
                        else return qiew{};                                                 \
                    });                                                                     \
                    return stream::valid;                                                   \
                }                                                                           \
                void wipe()                                                                 \
                {                                                                           \
                    SEQ_WIPE_macro(WRAP_macro(struct_members))                              \
                    stream::reset();                                                        \
                }                                                                           \
                friend std::ostream& operator << (std::ostream& s,                          \
                                             CAT_macro(struct_name, _t) const& o)           \
                {                                                                           \
                    s << #struct_name " {";                                                 \
                    SEQ_LOGS_macro(WRAP_macro(struct_members))                              \
                    s << " }";                                                              \
                    return s;                                                               \
                }                                                                           \
            };                                                                              \
            using struct_name = wrapper<CAT_macro(struct_name, _t)>;

        //todo use C++20 __VA_OPT__ (MSVC not ready yet)
        #define STRUCT_macro_lite(struct_name)                                    \
            struct CAT_macro(struct_name, _t) : public stream                     \
            {                                                                     \
                static constexpr auto kind = type{ __COUNTER__ - _counter_base }; \
                CAT_macro(struct_name, _t)()                                      \
                    : stream{ kind }                                              \
                { }                                                               \
                void set() {}                                                     \
                void get(view& /*_data*/) {}                                      \
                                                                                  \
                friend std::ostream& operator << (std::ostream& s,                \
                                         CAT_macro(struct_name, _t) const& /*o*/) \
                {                                                                 \
                    return s << #struct_name " { }";                              \
                }                                                                 \
            };                                                                    \
            using struct_name = wrapper<CAT_macro(struct_name, _t)>;

        auto& operator << (std::ostream& s, wchr const& o) { return s << utf::to_hex_0x(o); }
        auto& operator << (std::ostream& s, time const& o) { return s << utf::to_hex_0x(o.time_since_epoch().count()); }
        auto& operator << (std::ostream& s, regs const& rs) { s << '{'; for (auto& r : rs) s << r; return s << '}'; }
        auto& operator << (std::ostream& s, many const& my) { s << '{'; for (auto& r : my) s << r.type().name(); return s << '}'; }

        STRUCT_macro(frame_element,     (blob, data))
        STRUCT_macro(jgc_element,       (ui64, token) (text, cluster))
        STRUCT_macro(tooltip_element,   (id_t, gear_id) (text, tip_text) (bool, update))
        STRUCT_macro(mouse_event,       (id_t, gear_id)
                                        (si32, ctlstat)
                                        (hint, cause)
                                        (fp2d, coord)
                                        (fp2d, delta)
                                        (si32, buttons)
                                        (fp32, whlfp)
                                        (si32, whlsi)
                                        (bool, hzwhl)
                                        (fp2d, click))
        STRUCT_macro(fullscrn,          (id_t, gear_id))
        STRUCT_macro(maximize,          (id_t, gear_id))
        STRUCT_macro(minimize,          (id_t, gear_id))
        STRUCT_macro(header,            (id_t, window_id) (text, utf8))
        STRUCT_macro(footer,            (id_t, window_id) (text, utf8))
        STRUCT_macro(header_request,    (id_t, window_id))
        STRUCT_macro(footer_request,    (id_t, window_id))
        STRUCT_macro(warping,           (id_t, window_id) (dent, warpdata))
        STRUCT_macro(command,           (text, utf8))
        STRUCT_macro(logs,              (ui32, id) (time, guid) (text, data))
        STRUCT_macro_lite(expose)
        STRUCT_macro(clipdata,          (id_t, gear_id) (time, hash) (twod, size) (text, utf8) (si32, form) (text, meta))
        STRUCT_macro(clipdata_request,  (id_t, gear_id) (time, hash))
        STRUCT_macro(sysboard,          (id_t, gear_id) (twod, size) (text, utf8) (si32, form))
        STRUCT_macro_lite(sysstart)
        STRUCT_macro(sysclose,          (bool, fast))
        STRUCT_macro(syswinsz,          (id_t, gear_id) (twod, winsize))
        STRUCT_macro(sysfocus,          (id_t, gear_id) (bool, state) (si32, focus_type) (si64, treeid) (ui64, digest))
        STRUCT_macro(syskeybd,          (id_t, gear_id)  // syskeybd: Devide id.
                                        (si32, ctlstat)  // syskeybd: Keybd modifiers.
                                        (time, timecod)  // syskeybd: Event time code.
                                        (si32, virtcod)  // syskeybd: Key virtual code.
                                        (si32, scancod)  // syskeybd: Scancode.
                                        (si32, keystat)  // syskeybd: Key state: unknown, pressed, repeated, released.
                                        (si32, keycode)  // syskeybd: Key id.
                                        (byte, payload)  // syskeybd: Payload type.
                                        (bool, extflag)  // syskeybd: Win32 extflag.
                                        (bool, handled)  // syskeybd: Key event is handled.
                                        (si64, touched)  // syskeybd: Key event is touched.
                                        (text, cluster)  // syskeybd: Generated string.
                                        (text, vkchord)  // sysmouse: Key virtcode-based chord.
                                        (text, scchord)  // sysmouse: Key scancode-based chord.
                                        (text, chchord)) // sysmouse: Key virtcode+cluster-based chord.
        STRUCT_macro(sysmouse,          (id_t, gear_id)  // sysmouse: Devide id.
                                        (si32, ctlstat)  // sysmouse: Keybd modifiers.
                                        (time, timecod)  // sysmouse: Event time code.
                                        (si32, enabled)  // sysmouse: Mouse device health status.
                                        (si32, buttons)  // sysmouse: Buttons bit state.
                                        (bool, hzwheel)  // sysmouse: If true: Horizontal scroll wheel. If faux: Vertical scroll wheel.
                                        (fp32, wheelfp)  // sysmouse: Scroll delta in floating units.
                                        (si32, wheelsi)  // sysmouse: Scroll delta in integer units.
                                        (fp2d, coordxy)  // sysmouse: Pixel-wise cursor coordinates.
                                        (ui32, changed)) // sysmouse: Update stamp.
        STRUCT_macro(mousebar,          (bool, mode)) // CCC_SMS/* 26:1p */
        STRUCT_macro(unknown_gc,        (ui64, token))
        STRUCT_macro(fps,               (si32, frame_rate))
        STRUCT_macro(init,              (text, user) (si32, mode) (text, env) (text, cwd) (text, cmd) (text, cfg) (twod, win))
        STRUCT_macro(cwd,               (text, path))
        STRUCT_macro(restored,          (id_t, gear_id))
        STRUCT_macro(req_input_fields,  (id_t, gear_id) (si32, acpStart) (si32, acpEnd))
        STRUCT_macro(ack_input_fields,  (id_t, gear_id) (regs, field_list))
        STRUCT_macro(gui_command,       (id_t, gear_id) (si32, cmd_id) (many, args))

        #undef STRUCT_macro
        #undef STRUCT_macro_lite
        #define UNDEFINE_macro
        #include "macrogen.hpp"

        struct bitmap_dtvt_t
            : public stream
        {
            static constexpr auto kind = type{ __COUNTER__ - _counter_base };

            bitmap_dtvt_t()
                : stream{ kind }
            { }

            cell                           state; // bitmap: .
            core                           image; // bitmap: .

            enum : byte
            {
                refer = 1 << 0, // 1 - Diff with our canvas cell, 0 - diff with current brush (state).
                bgclr = 1 << 1,
                fgclr = 1 << 2,
                style = 1 << 3,
                rastr = 1 << 4,
                glyph = 1 << 5,
                dmax  = 1 << 6,
            };

            struct subtype
            {
                static constexpr auto nop = byte{ 0x00 }; // Apply current brush. nop = dif - refer.
                static constexpr auto dif = byte{ dmax }; // Cell dif.
                static constexpr auto mov = byte{ 0xFE }; // Set insertion point. sz_t: offset.
                static constexpr auto rep = byte{ 0xFF }; // Repeat current brush ui32 times. sz_t: N.
            };

            void set(id_t winid, twod coord, core& cache, flag& abort, sz_t& delta)
            {
                //todo multiple windows
                stream::reinit(winid, rect{ coord, cache.size() });
                auto pen = state;
                auto src = cache.begin();
                auto end = cache.end();
                auto csz = cache.size();
                auto fsz = image.size();
                auto dst = image.begin();
                auto dtx = fsz.x - csz.x;
                auto min = std::min(csz, fsz);
                if (src == end)
                {
                    delta = {};
                    return;
                }
                auto beg = src + 1;
                auto mid = src + csz.x * min.y;
                bool bad = true;
                auto sum = sz_t{ 0 };
                auto rep = [&]
                {
                    if (sum < sizeof(subtype::rep) + sizeof(sum))
                    {
                        do add(subtype::nop);
                        while (--sum);
                    }
                    else
                    {
                        add(subtype::rep, sum);
                        sum = 0;
                    }
                };
                auto tax = [](cell c1, cell const& c2)
                {
                    auto meaning = 0;
                    auto cluster = byte{ 0 };
                    auto changes = byte{ 0 };
                    if (c1.bgc() != c2.bgc()) { meaning += sizeof(c1.bgc()); changes |= bgclr; }
                    if (c1.fgc() != c2.fgc()) { meaning += sizeof(c1.fgc()); changes |= fgclr; }
                    if (c1.stl() != c2.stl()) { meaning += sizeof(c1.stl()); changes |= style; }
                    if (c1.img() != c2.img()) { meaning += sizeof(c1.img()); changes |= rastr; }
                    if (c1.egc() != c2.egc())
                    {
                        cluster = (byte)c1.len();
                        meaning += cluster + 1;
                        changes |= glyph;
                    }
                    return std::tuple{ meaning, changes, cluster };
                };
                auto dif = [&](byte changes, byte cluster, cell const& cache)
                {
                    add(changes);
                    if (changes & bgclr) add(cache.bgc());
                    if (changes & fgclr) add(cache.fgc());
                    if (changes & style) add(cache.stl());
                    if (changes & rastr) add(cache.img());
                    if (changes & glyph) add(cluster, cache.egc().glyph, cluster);
                    state = cache;
                };
                auto map = [&](auto const& cache, auto const& front)
                {
                    if (cache != front)
                    {
                        if (bad)
                        {
                            if (sum) rep();
                            auto offset = (sz_t)(src - beg);
                            add(subtype::mov, offset);
                            bad = faux;
                        }
                        if (cache == state) ++sum;
                        else
                        {
                            if (sum) rep();
                            auto [s_meaning, s_changes, s_cluster] = tax(cache, state);
                            auto [f_meaning, f_changes, f_cluster] = tax(cache, front);
                            if (s_meaning < f_meaning) dif(s_changes,         s_cluster, cache);
                            else                       dif(f_changes | refer, f_cluster, cache);
                        }
                    }
                    else bad = true;
                };
                while (src != mid && !abort)
                {
                    auto stop = src + min.x;
                    while (src != stop) map(*src++, *dst++);
                    if (dtx >= 0) dst += dtx;
                    else
                    {
                        stop += -dtx;
                        while (src != stop) map(*src++, pen);
                    }
                }
                if (csz.y > fsz.y)
                {
                    while (src != end && !abort) map(*src++, pen);
                }
                if (sum) rep();
                if (abort)
                {
                    std::swap(state, pen);
                    sum = reset();
                }
                else
                {
                    auto discard_empty = fsz == csz;
                    std::swap(image, cache);
                    sum = commit(discard_empty);
                }
                delta = sum;
            }
            template<class P = noop, class S = noop>
            void get(view& data, P update = {}, S resize = {})
            {
                auto [myid, area] = stream::take<id_t, rect>(data);
                //todo head.myid
                if (image.size() != area.size)
                {
                    image.crop(area.size);
                    resize(area.size);
                }
                auto mark = image.mark();
                auto head = image.begin();
                auto tail = image.end();
                auto iter = head;
                auto step = head;
                auto take = [&](auto what, cell& c)
                {
                    if (what & bgclr) stream::take(c.bgc(), data);
                    if (what & fgclr) stream::take(c.fgc(), data);
                    if (what & style) stream::take(c.stl(), data);
                    if (what & rastr) stream::take(c.img(), data);
                    if (what & glyph)
                    {
                        auto& gc = c.egc();
                        gc.token = 0;
                        auto [size] = stream::take<byte>(data);
                        stream::take(gc.glyph, size, data);
                        c.jgc(); // Check unknown jumbo clusters.
                    }
                    return c;
                };
                //auto frame_len = data.size();
                //auto nop_count = 0;
                //auto rep_count = 0;
                //auto mov_count = 0;
                //auto dif_count = 0;
                while (data.size() > 0)
                {
                    auto [what] = stream::take<byte>(data);
                    if (what == subtype::nop)
                    {
                        //nop_count++;
                        *iter++ = mark;
                    }
                    else if (what < subtype::dif)
                    {
                        //dif_count++;
                        auto& dest = *iter++;
                        if (what & refer) mark = take(what, dest);
                        else              dest = take(what, mark);
                    }
                    else if (what == subtype::rep)
                    {
                        //rep_count++;
                        auto [count] = stream::take<sz_t>(data);
                        auto upto = iter + count;
                        if (upto > tail)
                        {
                            log(prompt::dtvt, "bitmap: ", "Corrupted data, subtype: ", what);
                            break;
                        }
                        std::fill(iter, upto, mark);
                        iter = upto;
                    }
                    else if (what == subtype::mov)
                    {
                        //mov_count++;
                        auto [offset] = stream::take<sz_t>(data);
                        auto dest = head + offset;
                        if (dest >= tail)
                        {
                            log(prompt::dtvt, "bitmap: ", "Corrupted data, subtype: ", what);
                            break;
                        }
                        if constexpr (!std::is_same_v<P, noop>)
                        {
                            if (step != iter) update(head, step, iter);
                            step = dest;
                        }
                        iter = dest;
                    }
                    else // Unknown subtype.
                    {
                        log(prompt::dtvt, "bitmap: ", "Unknown data, subtype: ", what);
                        break;
                    }
                }
                image.mark(mark);
                if constexpr (!std::is_same_v<P, noop>)
                {
                    update(head, step, iter);
                }
                //log(prompt::dtvt, "frame len: ", frame_len);
                //log(prompt::dtvt, "nop count: ", nop_count);
                //log(prompt::dtvt, "rep count: ", rep_count);
                //log(prompt::dtvt, "dif count: ", dif_count);
                //log("----------------------------");
            }
        };
        template<svga Mode, type Kind>
        struct bitmap_a
            : public stream
        {
            static constexpr auto kind = Kind;

            cell state; // bitmap_a: .
            core image; // bitmap_a: .

            bitmap_a()
                : stream{ Kind }
            { }

            void set(id_t /*winid*/, twod /*winxy*/, core& cache, flag& abort, sz_t& delta)
            {
                auto coord = dot_00;
                auto saved = state;
                auto field = cache.size();
                auto mov = [&](auto x)
                {
                    coord.x = static_cast<decltype(coord.x)>(x);
                    block.basevt::locate(coord);
                };
                auto put = [&](cell const& cache)
                {
                    if (cache.cur())
                    {
                        auto c = cache;
                        c.draw_cursor();
                        c.scan<Mode>(state, block);
                    }
                    else cache.scan<Mode>(state, block);
                };
                auto dif = [&](cell const& cache, cell const& front)
                {
                    auto same = cache.check_pair(front);
                    if (same) put(cache);
                    return !same;
                };
                auto left_half = [&](cell const& cache)
                {
                    auto temp = cache;
                    temp.txt(cache.get_c0_left());
                    put(temp);
                };
                auto right_half = [&](cell const& cache)
                {
                    auto temp = cache;
                    temp.txt(cache.get_c0_right());
                    put(temp);
                };
                auto tie = [&](cell const& left, cell const& right)
                {
                    if (dif(left, right))
                    {
                        left_half(left);
                        right_half(right);
                    }
                };
                if (image.hash() != cache.hash())
                {
                    block.basevt::scroll_wipe();
                    auto src = cache.begin();
                    while (coord.y < field.y)
                    {
                        if (abort)
                        {
                            delta = reset();
                            state = saved;
                            break;
                        }
                        block.basevt::locate(coord);
                        auto end = src + field.x;
                        while (src != end)
                        {
                            auto& c = *src++;
                            auto [w, h, x, y] = c.whxy();
                            if (w < 2 && x < 2) put(c);
                            else
                            {
                                if (w == 2 && x == 1)
                                {
                                    if (src != end)
                                    {
                                        auto& right = *src;
                                        auto [rw, rh, rx, ry] = right.whxy();
                                        if (rx == 1) left_half(c);
                                        else
                                        {
                                            if (dif(c, right)) left_half(c);
                                            else               ++src;
                                        }
                                    }
                                    else left_half(c);
                                }
                                else right_half(c);
                            }
                        }
                        ++coord.y;
                    }
                    std::swap(image, cache);
                    delta = commit(true);
                }
                else
                {
                    auto src = cache.begin();
                    auto dst = image.begin();
                    while (coord.y < field.y)
                    {
                        if (abort)
                        {
                            delta = reset();
                            state = saved;
                            break;
                        }
                        auto beg = src + 1;
                        auto end = src + field.x;
                        while (src != end)
                        {
                            auto& fore = *src++;
                            auto& back = *dst++;
                            auto [w, h, x, y] = fore.whxy();
                            if (w < 2)
                            {
                                if (back != fore)
                                {
                                    mov(src - beg);
                                    put(fore);
                                    while (src != end)
                                    {
                                        auto& f = *src++;
                                        auto& b = *dst++;
                                        //auto fw = f.wdt();
                                        auto [fw, fh, fx, fy] = f.whxy();
                                        if (fw < 2)
                                        {
                                            if (b == f) break;
                                            else        put(f);
                                        }
                                        else if (fw == 2 && fx == 1) // Check left part.
                                        {
                                            if (src != end)
                                            {
                                                auto& right = *src;
                                                if (b == f && right == *dst)
                                                {
                                                    ++src;
                                                    ++dst;
                                                    break;
                                                }
                                                else
                                                {
                                                    auto [rw, rh, rx, ry] = right.whxy();
                                                    if (rx == 1) left_half(f);
                                                    else // right.wdt() == 3
                                                    {
                                                        tie(f, right);
                                                        ++src;
                                                        ++dst;
                                                    }
                                                }
                                            }
                                            else left_half(f);
                                        }
                                        else right_half(f); // fw == 3
                                    }
                                }
                            }
                            else
                            {
                                if (w == 2) // Left part has changed.
                                {
                                    if (back != fore)
                                    {
                                        mov(src - beg);
                                        if (src != end)
                                        {
                                            auto& right = *src;
                                            auto [rw, rh, rx, ry] = right.whxy();
                                            if (rx == 1) left_half(fore);
                                            else // right.wdt() == 3
                                            {
                                                tie(fore, right);
                                                ++src;
                                                ++dst;
                                            }
                                        }
                                        else left_half(fore);
                                    }
                                    else // Check right part.
                                    {
                                        if (src != end)
                                        {
                                            auto& right = *src;
                                            auto [rw, rh, rx, ry] = right.whxy();
                                            if (rx == 1) mov(src - beg), left_half(fore);
                                            else // right.wdt() == 3
                                            {
                                                if (right != *dst)
                                                {
                                                    mov(src - beg);
                                                    tie(fore, right);
                                                }
                                                ++src;
                                                ++dst;
                                            }
                                        }
                                        else mov(src - beg), left_half(fore);
                                    }
                                }
                                else mov(src - beg), right_half(fore); // w == 3
                            }
                        }
                        ++coord.y;
                    }
                    std::swap(image, cache);
                    delta = commit(true);
                }
            }
            void get(view& /*data*/) { }
        };
        struct bitmap_vtrgb_t : bitmap_a<svga::vtrgb, __COUNTER__ - _counter_base> { };
        struct bitmap_vt256_t : bitmap_a<svga::vt256, __COUNTER__ - _counter_base> { };
        struct bitmap_vt16_t  : bitmap_a<svga::vt16,  __COUNTER__ - _counter_base> { };

        using bitmap_dtvt  = wrapper<bitmap_dtvt_t>;
        using bitmap_vtrgb = wrapper<bitmap_vtrgb_t>;
        using bitmap_vt256 = wrapper<bitmap_vt256_t>;
        using bitmap_vt16  = wrapper<bitmap_vt16_t>;
        using frames_t     = list<view,   frame_element_t>;
        using jgc_list_t   = list<text,     jgc_element_t>;
        using tooltips_t   = list<text, tooltip_element_t>;
        using request_gc_t = list<text, unknown_gc_t>;
        using frames       = wrapper<frames_t  >;
        using jgc_list     = wrapper<jgc_list_t>;
        using tooltips     = wrapper<tooltips_t>;
        using request_gc   = wrapper<request_gc_t>;

        struct s11n
        {
            #define object_list \
            X(bitmap_dtvt      ) /* Canvas in dtvt format.                        */\
            X(bitmap_vtrgb     ) /* Canvas in truecolor format.                   */\
            X(bitmap_vt256     ) /* Canvas in 256-color format.                   */\
            X(bitmap_vt16      ) /* Canvas in 16-color format.                    */\
            X(mouse_event      ) /* Mouse events.                                 */\
            X(tooltips         ) /* Tooltip list.                                 */\
            X(jgc_list         ) /* List of jumbo GC.                             */\
            X(fullscrn         ) /* Notify/Request to fullscreen.                 */\
            X(maximize         ) /* Request to maximize window.                   */\
            X(header           ) /* Set window title.                             */\
            X(footer           ) /* Set window footer.                            */\
            X(header_request   ) /* Request window title.                         */\
            X(footer_request   ) /* Request window footer.                        */\
            X(warping          ) /* Warp resize.                                  */\
            X(minimize         ) /* Minimize window.                              */\
            X(expose           ) /* Bring window to the front.                    */\
            X(command          ) /* Interactive command/result in UTF-8 format.   */\
            X(frames           ) /* Received frames.                              */\
            X(tooltip_element  ) /* Tooltip text.                                 */\
            X(jgc_element      ) /* jumbo GC: gc.token + gc.view.                 */\
            X(logs             ) /* Debug logs.                                   */\
            X(syskeybd         ) /* System keybd device.                          */\
            X(sysmouse         ) /* System mouse device.                          */\
            X(sysfocus         ) /* System focus device.                          */\
            X(sysstart         ) /* System start event.                           */\
            X(sysclose         ) /* System close event.                           */\
            X(syswinsz         ) /* Console window resize.                        */\
            X(sysboard         ) /* Clipboard preview.                            */\
            X(clipdata         ) /* Clipboard raw data.                           */\
            X(clipdata_request ) /* Request clipboard data.                       */\
            X(mousebar         ) /* Show mouse cursor.                            */\
            X(request_gc       ) /* Unknown gc token list.                        */\
            X(unknown_gc       ) /* Unknown gc token.                             */\
            X(fps              ) /* Set frame rate.                               */\
            X(init             ) /* Startup data.                                 */\
            X(cwd              ) /* CWD Notification.                             */\
            X(restored         ) /* Notify normal window state.                   */\
            X(req_input_fields ) /* Request input field list.                     */\
            X(ack_input_fields ) /* Reply input field list.                       */\
            X(gui_command      ) /* GUI command request.                          */
            //X(quit             ) /* Close and disconnect dtvt app.                */
            //X(focus            ) /* Request to set focus.                         */

            struct xs
            {
                #define X(_object) using _object = binary::_object::access;
                object_list
                #undef X
            };

            #define X(_object) binary::_object _object;
            object_list
            #undef X

            std::unordered_map<type, std::function<void(view&)>> exec; // s11n: .
            escx s11n_output; // s11n: Logs buffer.
            escx s11n_logpad; // s11n: Logs left margin.

            // s11n: Deserialize objects.
            void sync(view& data)
            {
                auto lock = frames.sync(data);
                for(auto& frame : lock.thing)
                {
                    auto iter = exec.find(frame.next);
                    if (iter != exec.end())
                    {
                        iter->second(frame.data);
                    }
                    else log(prompt::s11n, "Unsupported frame type: ", (int)frame.next, "\n", utf::debase(frame.data));
                }
            }
            // s11n: Request jumbo clusters (after received bitmap synchronization).
            void request_jgc(auto& master)
            {
                auto jumbos = cell::glyf::jumbos();
                if (jumbos.unk.size())
                {
                    auto list = s11n::request_gc.freeze();
                    for (auto& token : jumbos.unk)
                    {
                        list.thing.push(token);
                    }
                    jumbos.unk.clear();
                    list.thing.sendby(master);
                }
            }
            // s11n: Receive jumbo clusters.
            void receive_jgc(s11n::xs::jgc_list& lock)
            {
                auto jumbos = cell::glyf::jumbos();
                for (auto& jgc : lock.thing)
                {
                    jumbos.set(jgc.token, jgc.cluster);
                    if constexpr (debugmode) log(prompt::s11n, "New gc token: ", utf::to_hex_0x(jgc.token), " cluster size ", jgc.cluster.size(), " data: ", jgc.cluster);
                }
            }
            // s11n: Recycle logs.
            void recycle_log(s11n::xs::logs& lock, auto process_guid)
            {
                if (lock.thing.guid != process_guid) // To avoid overflow on recursive dtvt connections.
                {
                    auto utf8 = view{ lock.thing.data };
                    if (utf8.size() && utf8.back() == '\n') utf8.remove_suffix(1);
                    if (lock.thing.id == 0) // Message from our process.
                    {
                        log(utf8);
                    }
                    else
                    {
                        s11n_logpad.add(prompt::pads, lock.thing.id, ": "); // Local host pid and remote host pid can be different. It is different if sshed.
                        utf::split(utf8, '\n', [&](auto line)
                        {
                            s11n_output.add(s11n_logpad, line, '\n');
                        });
                        log<faux>(s11n_output);
                        s11n_output.clear();
                        s11n_logpad.clear();
                    }
                }
            }
            // s11n: Recycle clipboard request.
            void recycle_cliprequest(auto& master, s11n::xs::clipdata_request& lock)
            {
                auto& item = lock.thing;
                auto data = s11n::clipdata.freeze();
                if (data.thing.hash != item.hash)
                {
                    data.thing.template sendby<faux, faux>(master); //todo gcc 11.4.0 requires template keyword
                }
                else // Send without payload if hash the same.
                {
                    auto temp = std::move(data.thing.utf8);
                    data.thing.set();
                    data.thing.template sendby<faux, faux>(master); //todo gcc 11.4.0 requires template keyword
                    data.thing.utf8 = std::move(temp);
                    data.thing.set();
                }
            }
            // s11n: Wake up waiting objects.
            void stop(bool alive = faux)
            {
                #define X(_object) if constexpr (requires{ _object.freeze(); }) _object.freeze().alive = alive;
                object_list
                #undef X
            }

            s11n() = default;
            s11n(auto& boss, id_t boss_id = {})
            {
                #define X(_object) \
                    if constexpr (requires(view data){ boss.direct(_object.freeze(), data); }) \
                        exec[binary::_object::kind] = [&](auto& data){ boss.direct(_object.freeze(), data); }; \
                    else if constexpr (requires(view data){ boss.handle(_object.sync(data)); }) \
                        exec[binary::_object::kind] = [&](auto& data){ boss.handle(_object.sync(data)); }; \
                    else \
                        exec[binary::_object::kind] = [&](auto& data){ _object.sync(data); }; // Notify on receiving.
                object_list
                #undef X
                auto lock = bitmap_dtvt.freeze();
                lock.thing.image.link(boss_id);
            }
            #undef object_list
        };
    }
}
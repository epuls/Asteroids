#pragma once

#include <Utilities/Delegate.hpp>


enum KeyState{PRESSED,HELD,RELEASED,EMPTY};

/**
 * \class OnKey
 * \brief Wrapper around a Delegate to allow for input event subscription/removal
 */
class OnKey{
public:
    using OnKeyEvent = Delegate<KeyState>;

    struct Entry{int order; OnKeyEvent fn;};

    template<auto Fn> void AddOnPressed(int order=0) {
        m_entries.push_back({order, OnKeyEvent ::FromFn<Fn>()});
        m_sorted=false;
    }

    template<auto Fn> void RemoveOnPressed(int order = 0) {
        OnKeyEvent target = OnKeyEvent::FromFn<Fn>();
        auto it = std::remove_if(m_entries.begin(), m_entries.end(),
                                 [&](const Entry& e){ return e.order == order && e.fn == target; });
        if (it != m_entries.end()) m_entries.erase(it, m_entries.end());
    }

    template<auto Mem, class T> void AddOnPressed(T* obj, int order= 0) {
        m_entries.push_back({order, OnKeyEvent ::FromMethod<Mem>(obj)});
        m_sorted=false;
    }

    template<auto Mem, class T> void RemoveOnPressed(T* obj, int order= 0){
        OnKeyEvent target = OnKeyEvent::FromMethod<Mem>(obj);
        auto it = std::remove_if(m_entries.begin(), m_entries.end(),
                                 [&](const Entry& e){ return e.order == order && e.fn == target; });
        if (it != m_entries.end()) m_entries.erase(it, m_entries.end());
    }

    void Invoke(KeyState keyState){
        if (!m_sorted) sort();
        for (auto& e : m_entries)e.fn(keyState);
    }

private:
    std::vector<Entry> m_entries;
    bool m_sorted{true};
    void sort(){ std::stable_sort(m_entries.begin(), m_entries.end(), [](auto&a, auto&b){return a.order < b.order;}); m_sorted=true; }
    void Clear(){ m_entries.clear(); m_sorted = false; };
};


/**
 * \class InputManager
 * \brief Manages input, requires manual creation of Key structs with a static event handler
 * \details Needs serious redesign, I think poorly designed
 */
class InputManager{
public:
    enum KeyCode{UP,DOWN,LEFT,RIGHT, SPACE, G};

    enum EventType{
        KEY_DOWN,
        KEY_UP
    };

    struct Key{
        explicit Key(KeyCode k) : key(k){state = EMPTY;};
        KeyCode key;
        KeyState state;
        OnKey onKey{};

    };

    Key UpKey{UP};
    Key LeftKey{LEFT};
    Key RightKey{RIGHT};
    Key SpaceKey{SPACE};
    Key GKey{G};


    static void KeyEventHandler(Key& k, EventType e){
        if (e == KEY_DOWN){
            switch(k.state){
                case(EMPTY):
                    k.state = PRESSED;
                    k.onKey.Invoke(PRESSED);
                    //std::cout << "Key Pressed First Time\n";
                    break;
                case(PRESSED):
                    k.state = HELD;
                    k.onKey.Invoke(HELD);
                    //std::cout << "Held First Time\n";
                    break;
                case(HELD):
                    //stay held
                    k.onKey.Invoke(HELD);
                    break;
            }
        } else if (e == KEY_UP){
            switch(k.state){
                case(EMPTY):
                    //std::cout << "Key up from empty key state, should not happen!\n";
                    k.onKey.Invoke(RELEASED);
                    break;
                default:
                    k.state = EMPTY;
                    //std::cout << "Key Released\n";
                    k.onKey.Invoke(RELEASED);

            }
        }
    }
};
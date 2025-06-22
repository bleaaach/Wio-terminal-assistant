#pragma once

#include <lvgl.h>

class PageManager;

class PageBase {
public:
    // Page state
    typedef enum {
        PAGE_STATE_IDLE,
        PAGE_STATE_LOAD,
        PAGE_STATE_WILL_APPEAR,
        PAGE_STATE_DID_APPEAR,
        PAGE_STATE_ACTIVITY,
        PAGE_STATE_WILL_DISAPPEAR,
        PAGE_STATE_DID_DISAPPEAR,
        PAGE_STATE_UNLOAD,
        _PAGE_STATE_LAST
    } State_t;

    // Animation properties
    typedef struct {
        uint8_t Type;
        uint16_t Time;
        lv_anim_path_cb_t Path;
    } AnimAttr_t;

public:
    lv_obj_t* _root;              // UI root node
    PageManager* _Manager;        // Page manager pointer
    const char* _Name;            // Page name
    uint16_t _ID;                 // Page ID
    void* _UserData;              // User data pointer

    // Private data, only page manager access
    struct {
        State_t State;            // Page state
        
        // Animation state
        struct {
            bool IsEnter;         // Whether it is the entering party
            bool IsBusy;          // Whether the animation is playing
            AnimAttr_t Attr;      // Animation properties
        } Anim;
    } priv;

public:
    PageBase() : _root(nullptr), _Manager(nullptr), _Name(nullptr), _ID(0), _UserData(nullptr) {
        priv.State = PAGE_STATE_IDLE;
        priv.Anim.IsEnter = false;
        priv.Anim.IsBusy = false;
        priv.Anim.Attr.Type = 0;
        priv.Anim.Attr.Time = 500;
        priv.Anim.Attr.Path = nullptr;
    }

    virtual ~PageBase() {}

    // Page lifecycle methods
    virtual void onViewLoad() {}
    virtual void onViewDidLoad() {}
    virtual void onViewWillAppear() {}
    virtual void onViewDidAppear() {}
    virtual void onViewWillDisappear() {}
    virtual void onViewDidDisappear() {}
    virtual void onViewUnload() {}
    virtual void onViewDidUnload() {}

    // Input handling
    virtual void onKey(lv_dir_t direction) {}
    virtual void onEncoder(int32_t diff) {}
    virtual void onButton(bool pressed) { (void)pressed; } // Default empty implementation
};

#pragma once

#include "PageBase.h"

#define MAX_PAGES 10
#define MAX_STACK_SIZE 10

class PageManager {
public:
    // Page switching animation type
    typedef enum {
        LOAD_ANIM_NONE = 0,
        LOAD_ANIM_OVER_LEFT,
        LOAD_ANIM_OVER_RIGHT,
        LOAD_ANIM_OVER_TOP,
        LOAD_ANIM_OVER_BOTTOM,
        LOAD_ANIM_MOVE_LEFT,
        LOAD_ANIM_MOVE_RIGHT,
        LOAD_ANIM_FADE_ON,
        _LOAD_ANIM_LAST
    } LoadAnim_t;

public:
    PageManager();
    ~PageManager();

    // Page management
    bool Register(PageBase* page, const char* name);
    bool Unregister(const char* name);
    
    // Navigation
    bool Push(const char* name);
    bool Pop();
    bool Replace(const char* name);
    bool BackHome();
    
    // Animation
    void SetGlobalLoadAnimType(LoadAnim_t anim = LOAD_ANIM_OVER_LEFT, uint16_t time = 500);
    
    // Input handling
    void HandleInput(lv_dir_t direction);
    void HandleEncoder(int32_t diff);
    void HandleButton(bool pressed);
    
    // Getters
    const char* GetCurrentPageName();
    PageBase* GetCurrentPage() { return _PageCurrent; }

private:
    // Page pool management
    PageBase* FindPageInPool(const char* name);
    
    // Page stack management
    PageBase* GetStackTop();
    void SetStackClear(bool keepBottom = false);
    
    // Page switching
    bool SwitchTo(PageBase* page, bool isEnterAct);
    static void onSwitchAnimFinish(lv_anim_t* a);
    void SwitchAnimCreate(PageBase* page);
    
    // State management
    PageBase::State_t StateLoadExecute(PageBase* page);
    PageBase::State_t StateWillAppearExecute(PageBase* page);
    PageBase::State_t StateDidAppearExecute(PageBase* page);
    PageBase::State_t StateWillDisappearExecute(PageBase* page);
    PageBase::State_t StateDidDisappearExecute(PageBase* page);
    PageBase::State_t StateUnloadExecute(PageBase* page);
    void StateUpdate(PageBase* page);

private:
    // Page pool (using simple arrays instead of std::vector)
    PageBase* _PagePool[MAX_PAGES];
    int _PagePoolSize;

    // Page stack (using simple array instead of std::stack)
    PageBase* _PageStack[MAX_STACK_SIZE];
    int _StackTop;

    // Current pages
    PageBase* _PagePrev;
    PageBase* _PageCurrent;

    // Animation state
    struct {
        bool IsSwitchReq;
        bool IsBusy;
        bool IsEntering;
        PageBase::AnimAttr_t Global;
    } _AnimState;
};

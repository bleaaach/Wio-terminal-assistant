#include "PageManager.h"
#include <Arduino.h>
#include <cstring>

PageManager::PageManager() {
    // Initialize page pool
    for (int i = 0; i < MAX_PAGES; i++) {
        _PagePool[i] = nullptr;
    }
    _PagePoolSize = 0;

    // Initialize page stack
    for (int i = 0; i < MAX_STACK_SIZE; i++) {
        _PageStack[i] = nullptr;
    }
    _StackTop = -1;

    _PagePrev = nullptr;
    _PageCurrent = nullptr;

    _AnimState.IsSwitchReq = false;
    _AnimState.IsBusy = false;
    _AnimState.IsEntering = false;
    _AnimState.Global.Type = LOAD_ANIM_OVER_LEFT;
    _AnimState.Global.Time = 500;
    _AnimState.Global.Path = lv_anim_path_ease_out;
}

PageManager::~PageManager() {
    // Clean up pages
    for (int i = 0; i < _PagePoolSize; i++) {
        if (_PagePool[i]) {
            if (_PagePool[i]->_root) {
                lv_obj_del(_PagePool[i]->_root);
            }
            delete _PagePool[i];
            _PagePool[i] = nullptr;
        }
    }
    _PagePoolSize = 0;
}

bool PageManager::Register(PageBase* page, const char* name) {
    if (!page || !name) {
        Serial.println("Error: Invalid page or name");
        return false;
    }

    if (_PagePoolSize >= MAX_PAGES) {
        Serial.println("Error: Page pool is full");
        return false;
    }

    // Check if page already exists
    if (FindPageInPool(name)) {
        Serial.printf("Warning: Page '%s' already registered\n", name);
        return false;
    }

    page->_Name = name;
    page->_Manager = this;
    page->_ID = _PagePoolSize;

    _PagePool[_PagePoolSize] = page;
    _PagePoolSize++;

    Serial.printf("Page '%s' registered (ID: %d)\n", name, page->_ID);
    return true;
}

bool PageManager::Unregister(const char* name) {
    for (int i = 0; i < _PagePoolSize; i++) {
        if (_PagePool[i] && strcmp(_PagePool[i]->_Name, name) == 0) {
            PageBase* page = _PagePool[i];

            // Clean up if it's current page
            if (page == _PageCurrent) {
                StateUnloadExecute(page);
                _PageCurrent = nullptr;
            }

            if (page->_root) {
                lv_obj_del(page->_root);
            }

            delete page;

            // Shift remaining pages
            for (int j = i; j < _PagePoolSize - 1; j++) {
                _PagePool[j] = _PagePool[j + 1];
            }
            _PagePool[_PagePoolSize - 1] = nullptr;
            _PagePoolSize--;

            Serial.printf("Page '%s' unregistered\n", name);
            return true;
        }
    }

    Serial.printf("Warning: Page '%s' not found for unregistration\n", name);
    return false;
}

bool PageManager::Push(const char* name) {
    PageBase* page = FindPageInPool(name);
    if (!page) {
        Serial.printf("Error: Page '%s' not found\n", name);
        return false;
    }

    Serial.printf("Pushing page: %s\n", name);

    // Push current page to stack if exists
    if (_PageCurrent && _StackTop < MAX_STACK_SIZE - 1) {
        _StackTop++;
        _PageStack[_StackTop] = _PageCurrent;
    }

    return SwitchTo(page, true);
}

bool PageManager::Pop() {
    if (_StackTop < 0) {
        Serial.println("Warning: Page stack is empty, cannot pop");
        return false;
    }

    PageBase* prevPage = _PageStack[_StackTop];
    _PageStack[_StackTop] = nullptr;
    _StackTop--;

    Serial.printf("Popping to page: %s\n", prevPage->_Name);

    return SwitchTo(prevPage, false);
}

bool PageManager::Replace(const char* name) {
    Serial.printf("PageManager::Replace called with name: %s\n", name);
    PageBase* page = FindPageInPool(name);
    if (!page) {
        Serial.printf("Error: Page '%s' not found in pool\n", name);
        Serial.printf("Available pages in pool (%d):\n", _PagePoolSize);
        for (int i = 0; i < _PagePoolSize; i++) {
            if (_PagePool[i]) {
                Serial.printf("  [%d] %s\n", i, _PagePool[i]->_Name);
            }
        }
        return false;
    }

    Serial.printf("Replacing with page: %s\n", name);

    // Don't push to stack, just replace
    bool result = SwitchTo(page, true);
    Serial.printf("SwitchTo result: %d\n", result);
    return result;
}

bool PageManager::BackHome() {
    if (_StackTop < 0) {
        Serial.println("Already at home page");
        return true;
    }

    // Clear stack and go to bottom page
    PageBase* homePage = _PageStack[0]; // Bottom of stack

    // Clear the stack
    for (int i = 0; i <= _StackTop; i++) {
        _PageStack[i] = nullptr;
    }
    _StackTop = -1;

    if (homePage) {
        Serial.printf("Going back to home: %s\n", homePage->_Name);
        return SwitchTo(homePage, false);
    }

    return false;
}

void PageManager::SetGlobalLoadAnimType(LoadAnim_t anim, uint16_t time) {
    _AnimState.Global.Type = anim;
    _AnimState.Global.Time = time;
    _AnimState.Global.Path = lv_anim_path_ease_out;
    
    Serial.printf("Global animation set: type=%d, time=%d\n", anim, time);
}

void PageManager::HandleInput(lv_dir_t direction) {
    if (_PageCurrent && _PageCurrent->priv.State == PageBase::PAGE_STATE_ACTIVITY) {
        _PageCurrent->onKey(direction);
    }
}

void PageManager::HandleEncoder(int32_t diff) {
    if (_PageCurrent && _PageCurrent->priv.State == PageBase::PAGE_STATE_ACTIVITY) {
        _PageCurrent->onEncoder(diff);
    }
}

void PageManager::HandleButton(bool pressed) {
    if (_PageCurrent && _PageCurrent->priv.State == PageBase::PAGE_STATE_ACTIVITY) {
        _PageCurrent->onButton(pressed);
    }
}

const char* PageManager::GetCurrentPageName() {
    return _PageCurrent ? _PageCurrent->_Name : "None";
}

PageBase* PageManager::FindPageInPool(const char* name) {
    for (int i = 0; i < _PagePoolSize; i++) {
        if (_PagePool[i] && strcmp(_PagePool[i]->_Name, name) == 0) {
            return _PagePool[i];
        }
    }
    return nullptr;
}

PageBase* PageManager::GetStackTop() {
    return (_StackTop >= 0) ? _PageStack[_StackTop] : nullptr;
}

void PageManager::SetStackClear(bool keepBottom) {
    if (keepBottom && _StackTop >= 0) {
        PageBase* bottom = _PageStack[0];
        for (int i = 1; i <= _StackTop; i++) {
            _PageStack[i] = nullptr;
        }
        _PageStack[0] = bottom;
        _StackTop = 0;
    } else {
        for (int i = 0; i <= _StackTop; i++) {
            _PageStack[i] = nullptr;
        }
        _StackTop = -1;
    }
}

bool PageManager::SwitchTo(PageBase* page, bool isEnterAct) {
    if (!page) {
        Serial.println("Error: Cannot switch to null page");
        return false;
    }

    if (_AnimState.IsBusy) {
        Serial.println("Warning: Animation is busy, switch request ignored");
        return false;
    }

    Serial.printf("Switching to page: %s (enter=%d)\n", page->_Name, isEnterAct);

    _PagePrev = _PageCurrent;
    _PageCurrent = page;
    _AnimState.IsEntering = isEnterAct;

    // Handle previous page
    if (_PagePrev && _PagePrev != _PageCurrent) {
        StateWillDisappearExecute(_PagePrev);
    }

    // Handle current page
    if (_PageCurrent->priv.State == PageBase::PAGE_STATE_IDLE) {
        StateLoadExecute(_PageCurrent);
    }

    StateWillAppearExecute(_PageCurrent);
    StateDidAppearExecute(_PageCurrent);

    // Handle previous page cleanup
    if (_PagePrev && _PagePrev != _PageCurrent) {
        StateDidDisappearExecute(_PagePrev);
    }

    return true;
}

void PageManager::onSwitchAnimFinish(lv_anim_t* a) {
    PageManager* manager = (PageManager*)a->user_data;
    if (manager) {
        manager->_AnimState.IsBusy = false;
        Serial.println("Switch animation finished");
    }
}

void PageManager::SwitchAnimCreate(PageBase* page) {
    // Simple implementation - just mark as not busy
    _AnimState.IsBusy = false;
}

PageBase::State_t PageManager::StateLoadExecute(PageBase* page) {
    if (!page) return PageBase::PAGE_STATE_IDLE;

    Serial.printf("Loading page: %s\n", page->_Name);

    page->priv.State = PageBase::PAGE_STATE_LOAD;

    // Create root object if not exists
    if (!page->_root) {
        page->_root = lv_obj_create(lv_scr_act());
        lv_obj_set_size(page->_root, LV_HOR_RES, LV_VER_RES);
        lv_obj_align(page->_root, LV_ALIGN_CENTER, 0, 0);
        lv_obj_clear_flag(page->_root, LV_OBJ_FLAG_SCROLLABLE);
    }

    page->onViewLoad();
    page->onViewDidLoad();

    return page->priv.State;
}

PageBase::State_t PageManager::StateWillAppearExecute(PageBase* page) {
    if (!page) return PageBase::PAGE_STATE_IDLE;

    Serial.printf("Page will appear: %s\n", page->_Name);

    page->priv.State = PageBase::PAGE_STATE_WILL_APPEAR;
    page->onViewWillAppear();

    // Show the page
    if (page->_root) {
        lv_obj_clear_flag(page->_root, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(page->_root);
    }

    return page->priv.State;
}

PageBase::State_t PageManager::StateDidAppearExecute(PageBase* page) {
    if (!page) return PageBase::PAGE_STATE_IDLE;

    Serial.printf("Page did appear: %s\n", page->_Name);

    page->priv.State = PageBase::PAGE_STATE_DID_APPEAR;
    page->onViewDidAppear();

    page->priv.State = PageBase::PAGE_STATE_ACTIVITY;

    return page->priv.State;
}

PageBase::State_t PageManager::StateWillDisappearExecute(PageBase* page) {
    if (!page) return PageBase::PAGE_STATE_IDLE;

    Serial.printf("Page will disappear: %s\n", page->_Name);

    page->priv.State = PageBase::PAGE_STATE_WILL_DISAPPEAR;
    page->onViewWillDisappear();

    return page->priv.State;
}

PageBase::State_t PageManager::StateDidDisappearExecute(PageBase* page) {
    if (!page) return PageBase::PAGE_STATE_IDLE;

    Serial.printf("Page did disappear: %s\n", page->_Name);

    page->priv.State = PageBase::PAGE_STATE_DID_DISAPPEAR;
    page->onViewDidDisappear();

    // Hide the page
    if (page->_root) {
        lv_obj_add_flag(page->_root, LV_OBJ_FLAG_HIDDEN);
    }

    return page->priv.State;
}

PageBase::State_t PageManager::StateUnloadExecute(PageBase* page) {
    if (!page) return PageBase::PAGE_STATE_IDLE;

    Serial.printf("Unloading page: %s\n", page->_Name);

    page->priv.State = PageBase::PAGE_STATE_UNLOAD;
    page->onViewUnload();

    if (page->_root) {
        lv_obj_del(page->_root);
        page->_root = nullptr;
    }

    page->onViewDidUnload();
    page->priv.State = PageBase::PAGE_STATE_IDLE;

    return page->priv.State;
}

void PageManager::StateUpdate(PageBase* page) {
    // State update logic can be added here if needed
}

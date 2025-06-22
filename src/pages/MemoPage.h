#pragma once

#include "../core/PageBase.h"
#include "../theme/style_manager.h"

class MemoPage : public PageBase {
public:
    MemoPage();
    virtual ~MemoPage();

    virtual void onViewLoad() override;
    virtual void onViewDidLoad() override;
    virtual void onViewWillAppear() override;
    virtual void onViewDidAppear() override;
    virtual void onViewWillDisappear() override;
    virtual void onViewDidDisappear() override;
    virtual void onViewUnload() override;

    virtual void onKey(lv_dir_t direction) override;
    virtual void onButton(bool pressed) override;

private:
    void createMemoUI();
    void updateMemoDisplay();
    void selectMemo(int index);
    void editMemo(int index);

private:
    lv_obj_t* titleLabel;
    lv_obj_t* memoGrid;
    lv_obj_t* memoCards[6];
    lv_obj_t* memoTitles[6];
    lv_obj_t* memoContents[6];
    lv_obj_t* memoIcons[6];
    
    int selectedMemo;
    bool viewMode; // true = view, false = grid
    
    struct Memo {
        const char* title;
        const char* content;
        const char* icon;
        lv_color_t color;
    };
    
    static Memo memos[6];
    static const int memoCount;
};

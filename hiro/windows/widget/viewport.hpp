#if defined(Hiro_Viewport)

namespace hiro {

struct pViewport : pWidget {
  Declare(Viewport, Widget)

  auto handle() const -> uintptr_t;
  auto setDroppable(bool droppable) -> void override;
  auto setFocusable(bool focusable) -> void override;

  auto doMouseLeave() -> void override;
  auto doMouseMove(s32 x, s32 y) -> void override;
  auto windowProc(HWND, UINT, WPARAM, LPARAM) -> maybe<LRESULT> override;
};

}

#endif

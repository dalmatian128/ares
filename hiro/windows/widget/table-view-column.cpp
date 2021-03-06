#if defined(Hiro_TableView)

namespace hiro {

auto pTableViewColumn::construct() -> void {
  if(auto parent = _parent()) {
    auto lock = parent->acquire();
    wchar_t text[] = L"";
    LVCOLUMN lvColumn{0};
    lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.iSubItem = self().offset();
    lvColumn.pszText = text;
    ListView_InsertColumn(parent->hwnd, self().offset(), &lvColumn);
    _setState();
  }
}

auto pTableViewColumn::destruct() -> void {
  if(auto parent = _parent()) {
    auto lock = parent->acquire();
    ListView_DeleteColumn(parent->hwnd, self().offset());
  }
}

auto pTableViewColumn::setActive() -> void {
  //unsupported
}

auto pTableViewColumn::setAlignment(Alignment alignment) -> void {
}

auto pTableViewColumn::setBackgroundColor(Color color) -> void {
}

auto pTableViewColumn::setEditable(bool editable) -> void {
  //unsupported
}

auto pTableViewColumn::setExpandable(bool expandable) -> void {
}

auto pTableViewColumn::setForegroundColor(Color color) -> void {
}

auto pTableViewColumn::setHorizontalAlignment(f64 alignment) -> void {
  _setState();
}

auto pTableViewColumn::setIcon(const image& icon) -> void {
  _setState();
}

auto pTableViewColumn::setResizable(bool resizable) -> void {
  _setState();
}

auto pTableViewColumn::setSorting(Sort sorting) -> void {
  _setState();
}

auto pTableViewColumn::setText(const string& text) -> void {
  _setState();
}

auto pTableViewColumn::setVerticalAlignment(f64 alignment) -> void {
}

auto pTableViewColumn::setWidth(s32 width) -> void {
  _setState();
}

auto pTableViewColumn::_parent() -> maybe<pTableView&> {
  if(auto parent = self().parentTableView()) {
    if(auto self = parent->self()) return *self;
  }
  return {};
}

auto pTableViewColumn::_setState() -> void {
  if(auto parent = _parent()) {
    auto lock = parent->acquire();
    parent->_setIcons();
    string text = state().text;
    if(state().sorting == Sort::Ascending ) text.append(" ^");
    if(state().sorting == Sort::Descending) text.append(" v");
    utf16_t wtext(text);
    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    lvColumn.fmt = LVCFMT_CENTER;
    lvColumn.iSubItem = self().offset();
    lvColumn.iImage = self().offset();
    lvColumn.pszText = wtext;
    lvColumn.cx = _width;
    if(state().horizontalAlignment < 0.333) lvColumn.fmt = LVCFMT_LEFT;
    if(state().horizontalAlignment > 0.666) lvColumn.fmt = LVCFMT_RIGHT;
    if(state().icon) lvColumn.mask |= LVCF_IMAGE;
    if(!state().resizable) lvColumn.fmt |= LVCFMT_FIXED_WIDTH;
    ListView_SetColumn(parent->hwnd, self().offset(), &lvColumn);
  }
}

}

#endif

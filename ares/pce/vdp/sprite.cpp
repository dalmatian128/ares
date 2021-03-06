auto VDC::Sprite::scanline(u32 y) -> void {
  objects.reset();
  if(!enable) return;

  y += 64;

  static const u32 widths [2] = {15, 31};
  static const u32 heights[4] = {15, 31, 63, 63};

  u32 count = 0;
  for(u32 index : range(64)) {
    n16 d0 = vdc->satb.read(index << 2 | 0);
    n16 d1 = vdc->satb.read(index << 2 | 1);
    n16 d2 = vdc->satb.read(index << 2 | 2);
    n16 d3 = vdc->satb.read(index << 2 | 3);

    Object object;
    object.y = d0.bit(0,9);
    object.height = heights[d3.bit(12,13)];
    if(y < object.y) continue;
    if(y > object.y + object.height) continue;

    object.x = d1.bit(0,9);
    object.characterMode = d2.bit(0);
    object.pattern = d2.bit(1,10);
    object.palette = d3.bit(0,3);
    object.priority = d3.bit(7);
    object.width = widths[d3.bit(8)];
    object.hflip = d3.bit(11);
    object.vflip = d3.bit(15);
    object.first = index == 0;

    if(object.width  == 31) object.pattern.bit(0) = 0;
    if(object.height == 31) object.pattern.bit(1) = 0;
    if(object.height == 63) object.pattern.bit(1,2) = 0;

    if(object.width == 15) {
      objects.append(object);
      if(++count >= 16) return vdc->irq.raise(VDC::IRQ::Line::Overflow);
    } else {
      //32-width sprites count as two 16-width sprite slots
      object.pattern ^= object.hflip;
      object.width = 15;
      objects.append(object);
      if(++count >= 16) return vdc->irq.raise(VDC::IRQ::Line::Overflow);

      object.x += 16;
      object.pattern ^= 1;
      objects.append(object);
      if(++count >= 16) return vdc->irq.raise(VDC::IRQ::Line::Overflow);
    }
  }
}

auto VDC::Sprite::run(u32 x, u32 y) -> void {
  color = 0;
  palette = 0;
  priority = 0;
  if(!enable) return;

  x += 32;
  y += 64;

  bool first = false;
  for(auto& object : objects) {
    if(x < object.x) continue;
    if(x > object.x + object.width) continue;

    n10 hoffset = x - object.x;
    n10 voffset = y - object.y;
    if(object.hflip) hoffset ^= object.width;
    if(object.vflip) voffset ^= object.height;

    n16 patternAddress = object.pattern;
    patternAddress  += (voffset >> 4) << 1;
    patternAddress  += (hoffset >> 4);
    patternAddress <<= 6;
    patternAddress  += (voffset & 15);

    n16 d0 = 0, d1 = 0, d2 = 0, d3 = 0;
    if(latch.vramMode != 1) {
      d0 = vdc->vram.read(patternAddress +  0);
      d1 = vdc->vram.read(patternAddress + 16);
      d2 = vdc->vram.read(patternAddress + 32);
      d3 = vdc->vram.read(patternAddress + 48);
    }
    if(latch.vramMode == 1) {
      if(object.characterMode == 0) {
        d0 = vdc->vram.read(patternAddress +  0);
        d1 = vdc->vram.read(patternAddress + 16);
      }
      if(object.characterMode == 1) {
        d0 = vdc->vram.read(patternAddress + 32);
        d1 = vdc->vram.read(patternAddress + 48);
      }
    }

    n4 index = 15 - (hoffset & 15);
    n4 color;
    color.bit(0) = d0.bit(index);
    color.bit(1) = d1.bit(index);
    color.bit(2) = d2.bit(index);
    color.bit(3) = d3.bit(index);
    if(!color) continue;

    if(this->color) {
      if(first) return vdc->irq.raise(VDC::IRQ::Line::Collision);
      return;
    }
    if(object.first) first = true;

    this->color = color;
    this->palette = object.palette;
    this->priority = object.priority;
  }
}

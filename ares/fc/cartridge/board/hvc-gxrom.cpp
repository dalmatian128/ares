struct HVC_GxROM : Interface {
  static auto create(string id) -> Interface* {
    if(id == "HVC-GNROM") return new HVC_GxROM(Revision::GNROM);
    if(id == "HVC-MHROM") return new HVC_GxROM(Revision::MHROM);
    return nullptr;
  }

  Memory::Readable<n8> programROM;
  Memory::Readable<n8> characterROM;
  Memory::Writable<n8> characterRAM;

  enum class Revision : u32 {
    GNROM,
    MHROM,
  } revision;

  HVC_GxROM(Revision revision) : revision(revision) {}

  auto load() -> void override {
    Interface::load(programROM, "program.rom");
    Interface::load(characterROM, "character.rom");
    Interface::load(characterRAM, "character.ram");
    mirror = pak->attribute("mirror") == "vertical";
  }

  auto save() -> void override {
    Interface::save(characterRAM, "character.ram");
  }

  auto readPRG(n32 address, n8 data) -> n8 override {
    if(address < 0x8000) return data;
    return programROM.read(programBank << 15 | (n15)address);
  }

  auto writePRG(n32 address, n8 data) -> void override {
    if(address < 0x8000) return;
    characterBank = data.bit(0,1);
    programBank = data.bit(4,5);
  }

  auto readCHR(n32 address, n8 data) -> n8 override {
    if(address & 0x2000) {
      address = address >> !mirror & 0x0400 | (n10)address;
      return ppu.readCIRAM(address);
    }
    address = characterBank << 13 | (n13)address;
    if(characterROM) return characterROM.read(address);
    if(characterRAM) return characterRAM.read(address);
    return data;
  }

  auto writeCHR(n32 address, n8 data) -> void override {
    if(address & 0x2000) {
      address = address >> !mirror & 0x0400 | (n10)address;
      return ppu.writeCIRAM(address, data);
    }
    address = characterBank << 13 | (n13)address;
    if(characterRAM) return characterRAM.write(address, data);
  }

  auto power() -> void override {
  }

  auto serialize(serializer& s) -> void override {
    s(characterRAM);
    s(mirror);
    s(programBank);
    s(characterBank);
  }

  n1 mirror;  //0 = horizontal, 1 = vertical
  n2 programBank;
  n2 characterBank;
};

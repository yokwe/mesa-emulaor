package mh.majuro.mesa.type;

import mh.majuro.mesa.Type.*;

public final class ExtraGlobalWord {
    public static final int SIZE = 1;

    // offset    0  size    1  type           name unused
    //   bit startBit  0  stopBit 10
    // offset    0  size    1  type boolean   name started
    //   bit startBit 11  stopBit 11
    public static final class started {
        public static final         int SIZE   =  1;
        public static final         int OFFSET =  0;
        public static final @CARD16 int MASK    = 0b0000_0000_0001_0000;
        public static final         int SHIFT   = 4;

        public static @LONG_POINTER int offset(@LONG_POINTER int base) {
            return base + OFFSET;
        }
        public static @CARD16 int getBit(@CARD16 int value) {
            return (value & MASK) >>> SHIFT;
        }
        public static @CARD16 int setBit(@CARD16 int value, @CARD16 int newValue) {
            return ((newValue << SHIFT) & MASK) | (value & ~MASK);
        }
        public static boolean get(@LONG_POINTER int base) {
            return RecordBase.getBitField(ExtraGlobalWord.started::offset, ExtraGlobalWord.started::getBit, base) != 0;
        }
        public static void set(@LONG_POINTER int base, boolean newValue) {
            RecordBase.setBitField(ExtraGlobalWord.started::offset, ExtraGlobalWord.started::setBit, base, (newValue ? 1 : 0));
        }
    }
    // offset    0  size    1  type boolean   name copy
    //   bit startBit 12  stopBit 12
    public static final class copy {
        public static final         int SIZE   =  1;
        public static final         int OFFSET =  0;
        public static final @CARD16 int MASK    = 0b0000_0000_0000_1000;
        public static final         int SHIFT   = 3;

        public static @LONG_POINTER int offset(@LONG_POINTER int base) {
            return base + OFFSET;
        }
        public static @CARD16 int getBit(@CARD16 int value) {
            return (value & MASK) >>> SHIFT;
        }
        public static @CARD16 int setBit(@CARD16 int value, @CARD16 int newValue) {
            return ((newValue << SHIFT) & MASK) | (value & ~MASK);
        }
        public static boolean get(@LONG_POINTER int base) {
            return RecordBase.getBitField(ExtraGlobalWord.copy::offset, ExtraGlobalWord.copy::getBit, base) != 0;
        }
        public static void set(@LONG_POINTER int base, boolean newValue) {
            RecordBase.setBitField(ExtraGlobalWord.copy::offset, ExtraGlobalWord.copy::setBit, base, (newValue ? 1 : 0));
        }
    }
    // offset    0  size    1  type boolean   name copied
    //   bit startBit 13  stopBit 13
    public static final class copied {
        public static final         int SIZE   =  1;
        public static final         int OFFSET =  0;
        public static final @CARD16 int MASK    = 0b0000_0000_0000_0100;
        public static final         int SHIFT   = 2;

        public static @LONG_POINTER int offset(@LONG_POINTER int base) {
            return base + OFFSET;
        }
        public static @CARD16 int getBit(@CARD16 int value) {
            return (value & MASK) >>> SHIFT;
        }
        public static @CARD16 int setBit(@CARD16 int value, @CARD16 int newValue) {
            return ((newValue << SHIFT) & MASK) | (value & ~MASK);
        }
        public static boolean get(@LONG_POINTER int base) {
            return RecordBase.getBitField(ExtraGlobalWord.copied::offset, ExtraGlobalWord.copied::getBit, base) != 0;
        }
        public static void set(@LONG_POINTER int base, boolean newValue) {
            RecordBase.setBitField(ExtraGlobalWord.copied::offset, ExtraGlobalWord.copied::setBit, base, (newValue ? 1 : 0));
        }
    }
    // offset    0  size    1  type boolean   name alloced
    //   bit startBit 14  stopBit 14
    public static final class alloced {
        public static final         int SIZE   =  1;
        public static final         int OFFSET =  0;
        public static final @CARD16 int MASK    = 0b0000_0000_0000_0010;
        public static final         int SHIFT   = 1;

        public static @LONG_POINTER int offset(@LONG_POINTER int base) {
            return base + OFFSET;
        }
        public static @CARD16 int getBit(@CARD16 int value) {
            return (value & MASK) >>> SHIFT;
        }
        public static @CARD16 int setBit(@CARD16 int value, @CARD16 int newValue) {
            return ((newValue << SHIFT) & MASK) | (value & ~MASK);
        }
        public static boolean get(@LONG_POINTER int base) {
            return RecordBase.getBitField(ExtraGlobalWord.alloced::offset, ExtraGlobalWord.alloced::getBit, base) != 0;
        }
        public static void set(@LONG_POINTER int base, boolean newValue) {
            RecordBase.setBitField(ExtraGlobalWord.alloced::offset, ExtraGlobalWord.alloced::setBit, base, (newValue ? 1 : 0));
        }
    }
    // offset    0  size    1  type boolean   name shared
    //   bit startBit 15  stopBit 15
    public static final class shared {
        public static final         int SIZE   =  1;
        public static final         int OFFSET =  0;
        public static final @CARD16 int MASK    = 0b0000_0000_0000_0001;
        public static final         int SHIFT   = 0;

        public static @LONG_POINTER int offset(@LONG_POINTER int base) {
            return base + OFFSET;
        }
        public static @CARD16 int getBit(@CARD16 int value) {
            return (value & MASK) >>> SHIFT;
        }
        public static @CARD16 int setBit(@CARD16 int value, @CARD16 int newValue) {
            return ((newValue << SHIFT) & MASK) | (value & ~MASK);
        }
        public static boolean get(@LONG_POINTER int base) {
            return RecordBase.getBitField(ExtraGlobalWord.shared::offset, ExtraGlobalWord.shared::getBit, base) != 0;
        }
        public static void set(@LONG_POINTER int base, boolean newValue) {
            RecordBase.setBitField(ExtraGlobalWord.shared::offset, ExtraGlobalWord.shared::setBit, base, (newValue ? 1 : 0));
        }
    }
}

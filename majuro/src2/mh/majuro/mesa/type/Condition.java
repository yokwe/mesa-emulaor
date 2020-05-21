package mh.majuro.mesa.type;

import mh.majuro.mesa.Type.*;

public final class Condition {
    public static final int SIZE = 1;

    // offset    0  size    1  type           name reserved
    //   bit startBit  0  stopBit  2
    // offset    0  size    1  type PsbIndex  name tail
    //   bit startBit  3  stopBit 12
    public static final class tail {
        public static final         int SIZE   =  1;
        public static final         int OFFSET =  0;
        public static final @CARD16 int MASK    = 0b0001_1111_1111_1000;
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
        public static @CARD16 int get(@LONG_POINTER int base) {
            return RecordBase.getBitField(Condition.tail::offset, Condition.tail::getBit, base);
        }
        public static void set(@LONG_POINTER int base, @CARD16 int newValue) {
            RecordBase.setBitField(Condition.tail::offset, Condition.tail::setBit, base, newValue);
        }
    }
    // offset    0  size    1  type           name available
    //   bit startBit 13  stopBit 13
    // offset    0  size    1  type boolean   name abortable
    //   bit startBit 14  stopBit 14
    public static final class abortable {
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
            return RecordBase.getBitField(Condition.abortable::offset, Condition.abortable::getBit, base) != 0;
        }
        public static void set(@LONG_POINTER int base, boolean newValue) {
            RecordBase.setBitField(Condition.abortable::offset, Condition.abortable::setBit, base, (newValue ? 1 : 0));
        }
    }
    // offset    0  size    1  type boolean   name wakeup
    //   bit startBit 15  stopBit 15
    public static final class wakeup {
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
            return RecordBase.getBitField(Condition.wakeup::offset, Condition.wakeup::getBit, base) != 0;
        }
        public static void set(@LONG_POINTER int base, boolean newValue) {
            RecordBase.setBitField(Condition.wakeup::offset, Condition.wakeup::setBit, base, (newValue ? 1 : 0));
        }
    }
}

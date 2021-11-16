#pragma once

using namespace NanoPb::Converter;

struct LOCAL_InnerMessage {
    enum class Type {
        UnionInnerOne,
        UnionInnerTwo,
        UnionInnerThree
    };
    virtual ~LOCAL_InnerMessage(){}
    virtual Type getType() const = 0;

    template<class T>
    T* as(){ return static_cast<T*>(this); }
    template<class T>
    const T* as() const { return static_cast<const T*>(this); }

    bool operator==(const LOCAL_InnerMessage &rhs) const {
        return rhs.getType() == getType();
    }

    bool operator!=(const LOCAL_InnerMessage &rhs) const {
        return !(rhs == *this);
    }
};

struct LOCAL_UnionInnerOne : public LOCAL_InnerMessage {
    int number = 0;

    LOCAL_UnionInnerOne() = default;
    LOCAL_UnionInnerOne(const LOCAL_UnionInnerOne&) = delete;
    LOCAL_UnionInnerOne(LOCAL_UnionInnerOne&&) = default;

    LOCAL_UnionInnerOne(int number) : number(number) {}

    Type getType() const override { return Type::UnionInnerOne; }

    bool operator==(const LOCAL_UnionInnerOne &rhs) const {
        return static_cast<const LOCAL_InnerMessage &>(*this) == static_cast<const LOCAL_InnerMessage &>(rhs) &&
               number == rhs.number;
    }

    bool operator!=(const LOCAL_UnionInnerOne &rhs) const {
        return !(rhs == *this);
    }
};

struct LOCAL_UnionInnerTwo : public LOCAL_InnerMessage {
    std::string str;

    LOCAL_UnionInnerTwo() = default;
    LOCAL_UnionInnerTwo(const LOCAL_UnionInnerTwo&) = delete;
    LOCAL_UnionInnerTwo(LOCAL_UnionInnerTwo&&) = default;

    LOCAL_UnionInnerTwo(const std::string &str) : str(str) {}

    Type getType() const override { return Type::UnionInnerTwo; }

    bool operator==(const LOCAL_UnionInnerTwo &rhs) const {
        return static_cast<const LOCAL_InnerMessage &>(*this) == static_cast<const LOCAL_InnerMessage &>(rhs) &&
               str == rhs.str;
    }

    bool operator!=(const LOCAL_UnionInnerTwo &rhs) const {
        return !(rhs == *this);
    }
};

struct LOCAL_UnionInnerThree : public LOCAL_InnerMessage {
    using ValuesContainer = std::vector<uint32_t>;
    ValuesContainer values;

    LOCAL_UnionInnerThree() = default;
    LOCAL_UnionInnerThree(const LOCAL_UnionInnerThree&) = delete;
    LOCAL_UnionInnerThree(LOCAL_UnionInnerThree&&) = default;

    LOCAL_UnionInnerThree(const ValuesContainer &values) : values(values) {}

    Type getType() const override { return Type::UnionInnerThree; }

    bool operator==(const LOCAL_UnionInnerThree &rhs) const {
        return static_cast<const LOCAL_InnerMessage &>(*this) == static_cast<const LOCAL_InnerMessage &>(rhs) &&
               values == rhs.values;
    }

    bool operator!=(const LOCAL_UnionInnerThree &rhs) const {
        return !(rhs == *this);
    }
};

/******************************************************************************************/

class UnionInnerOneConverter : public AbstractMessageConverter<
        UnionInnerOneConverter,
        LOCAL_UnionInnerOne,
        PROTO_UnionInnerOne,
        &PROTO_UnionInnerOne_msg>
{
public:
    static ProtoType encoderInit(const EncoderContext& ctx) {
        return ProtoType {
                .number = ctx.v.number
        };
    }

    static ProtoType decoderInit(DecoderContext& ctx){
        return ProtoType{
        };
    }

    static bool decoderApply(const ProtoType& proto, DecoderContext& ctx){
        ctx.v.number = proto.number;
        return true;
    }
};

class UnionInnerTwoConverter : public AbstractMessageConverter<
        UnionInnerTwoConverter,
        LOCAL_UnionInnerTwo,
        PROTO_UnionInnerTwo,
        &PROTO_UnionInnerTwo_msg>
{
public:
    static ProtoType encoderInit(const EncoderContext& ctx) {
        return ProtoType {
                .str = StringConverter::encoderInit(ctx.v.str)
        };
    }

    static ProtoType decoderInit(DecoderContext& ctx){
        return ProtoType{
                .str = StringConverter::decoderInit(ctx.v.str)
        };
    }

    static bool decoderApply(const ProtoType& proto, DecoderContext& ctx){
        return true;
    }
};

class UnionInnerThreeConverter : public AbstractMessageConverter<
        UnionInnerThreeConverter,
        LOCAL_UnionInnerThree,
        PROTO_UnionInnerThree,
        &PROTO_UnionInnerThree_msg>
{
public:
    static ProtoType encoderInit(const EncoderContext& ctx) {
        return ProtoType {
                .values = ArrayUnsignedConverter<LOCAL_UnionInnerThree::ValuesContainer>::encoderInit(ctx.v.values)
        };
    }

    static ProtoType decoderInit(DecoderContext& ctx){
        return ProtoType{
                .values = ArrayUnsignedConverter<LOCAL_UnionInnerThree::ValuesContainer>::decoderInit(ctx.v.values)
        };
    }

    static bool decoderApply(const ProtoType& proto, DecoderContext& ctx){
        return true;
    }
};
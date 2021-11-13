#include <vector>

#include "tests_common.h"
#include "array_message.pb.h"

using namespace NanoPb::Converter;

struct LOCAL_InnerMessage {
    uint32_t number = 0;
    std::string text;

    LOCAL_InnerMessage() = default; // Default constructor is required

    // Remove copy constructor and add move constructor
    // to ensure that all can work without copy constructor
    LOCAL_InnerMessage(const LOCAL_InnerMessage&) = delete;
    LOCAL_InnerMessage(LOCAL_InnerMessage&& other) : number(other.number), text(std::move(other.text)) {};

    LOCAL_InnerMessage(uint32_t number, const std::string &text) : number(number), text(text) {}

    bool operator==(const LOCAL_InnerMessage &rhs) const {
        return number == rhs.number &&
               text == rhs.text;
    }

    bool operator!=(const LOCAL_InnerMessage &rhs) const {
        return !(rhs == *this);
    }
};

struct LOCAL_OuterMessage {
    using ItemsContainer = std::vector<LOCAL_InnerMessage>;

    int32_t number = 0;
    ItemsContainer items;

    LOCAL_OuterMessage() = default;

    LOCAL_OuterMessage(int32_t number, ItemsContainer &&items) : number(number), items(std::move(items)) {}

    bool operator==(const LOCAL_OuterMessage &rhs) const {
        return number == rhs.number &&
               items == rhs.items;
    }

    bool operator!=(const LOCAL_OuterMessage &rhs) const {
        return !(rhs == *this);
    }
};

class InnerMessageConverter : public AbstractMessageConverter<InnerMessageConverter, LOCAL_InnerMessage, PROTO_InnerMessage, &PROTO_InnerMessage_msg> {
private:
    friend class AbstractMessageConverter;

    static ProtoType _encoderInit(const LocalType& local) {
        return ProtoType{
                .number = local.number,
                .text = StringConverter::encoder(local.text)
        };
    }

    static ProtoType _decoderInit(LocalType& local){
        return ProtoType{
            .text = StringConverter::decoder(local.text)
        };
    }

    static bool _decoderApply(const ProtoType& proto, LocalType& local){
        local.number = proto.number;
        return true;
    }
};


class OuterMessageConverter : public AbstractMessageConverter<OuterMessageConverter, LOCAL_OuterMessage, PROTO_OuterMessage, &PROTO_OuterMessage_msg> {
private:
    class ItemsConverter : public ArrayMessageConverter<
            ItemsConverter,
            LOCAL_OuterMessage::ItemsContainer ,
            InnerMessageConverter>
    {};
private:
    friend class AbstractMessageConverter;

    static ProtoType _encoderInit(const LocalType& local) {
        return ProtoType{
                .number = local.number,
                .items = ItemsConverter::encoder(local.items)
        };
    }

    static ProtoType _decoderInit(LocalType& local){
        return ProtoType{
                .items = ItemsConverter::decoder(local.items)
        };
    }

    static bool _decoderApply(const ProtoType& proto, LocalType& local){
        local.number = proto.number;
        return true;
    }
};


int main() {
    int status = 0;

    LOCAL_OuterMessage::ItemsContainer items;
    items.push_back(LOCAL_InnerMessage(1, "entry_1"));
    items.push_back(LOCAL_InnerMessage(2, "entry_1"));
    items.push_back(LOCAL_InnerMessage(UINT32_MAX, "entry_max"));

    const LOCAL_OuterMessage original (
            INT32_MIN,
            std::move(items)
    );

    NanoPb::StringOutputStream outputStream(STRING_BUFFER_STREAM_MAX_SIZE);

    TEST(NanoPb::encode<OuterMessageConverter>(outputStream, original));

    auto inputStream = NanoPb::StringInputStream(outputStream.release());

    LOCAL_OuterMessage decoded;

    TEST(NanoPb::decode<OuterMessageConverter>(inputStream, decoded));

    TEST(original == decoded);

    return status;
}

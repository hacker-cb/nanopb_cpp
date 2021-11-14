#include <utility>
#include <vector>
#include <list>

#include "tests_common.h"
#include "array_string.pb.h"

using namespace NanoPb::Converter;

struct LOCAL_TestMessage {
    using ContainerType = std::vector<std::string>;
    ContainerType values;

    LOCAL_TestMessage() = default;
    LOCAL_TestMessage(ContainerType values) : values(std::move(values)) {}

    bool operator==(const LOCAL_TestMessage &rhs) const {
        return values == rhs.values;
    }

    bool operator!=(const LOCAL_TestMessage &rhs) const {
        return !(rhs == *this);
    }
};

class TestMessageConverter : public BaseMessageConverter<
        TestMessageConverter,
        LOCAL_TestMessage,
        PROTO_TestMessage,
        &PROTO_TestMessage_msg>
{
public:
    static ProtoType encoderInit(const Context& ctx) {
        return ProtoType{
                .values = ArrayStringConverter<LOCAL_TestMessage::ContainerType>::encoder(ctx.values)
        };
    }

    static ProtoType decoderInit(Context& ctx){
        return ProtoType{
                .values = ArrayStringConverter<LOCAL_TestMessage::ContainerType>::decoder(ctx.values)
        };
    }

    static bool decoderApply(const ProtoType& proto, Context& ctx){
        return true;
    }
};


template <class CONTAINER>
int testRepeated(const typename CONTAINER::value_type minValue, const typename CONTAINER::value_type maxValue){
    int status = 0;

    LOCAL_TestMessage original({
        "string_1",
        "string_2",
        "string_3"
    });

    NanoPb::StringOutputStream outputStream(STRING_BUFFER_STREAM_MAX_SIZE);

    TEST(NanoPb::encode<TestMessageConverter>(outputStream, original));

    auto inputStream = NanoPb::StringInputStream(outputStream.release());

    LOCAL_TestMessage decoded;

    TEST(NanoPb::decode<TestMessageConverter>(inputStream, decoded));

    TEST(original == decoded);
    return status;
}

int main() {
    int status = 0;

    status |= testRepeated<std::vector<uint64_t>>(0, UINT64_MAX);
    status |= testRepeated<std::list<uint64_t>>(0, UINT64_MAX);

    status |= testRepeated<std::vector<uint32_t>>(0, UINT32_MAX);
    status |= testRepeated<std::list<uint32_t>>(0, UINT32_MAX);

    return status;
}

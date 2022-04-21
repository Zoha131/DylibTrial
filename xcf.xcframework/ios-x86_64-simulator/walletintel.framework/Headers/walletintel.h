#ifndef INCLUDEGUARD_4ce04acb_5791_4e27_b3ca_1fca8006ee0f
#define INCLUDEGUARD_4ce04acb_5791_4e27_b3ca_1fca8006ee0f

#include <stdio.h>

struct sessiontoken_value;
typedef struct sessiontoken_value* sessiontoken;

struct requesttoken_value;
typedef struct requesttoken_value* reqtoken;

enum shaktiNetwork {
    SHAKTI_MAINNET = 0,
    SHAKTI_TESTNET = 1
};

// A zone value of zero indicates the currency ledger, which is generally what
// you'll want. The values below indicate feat ledger zones.
enum shaktiZone {
    ZONE_AFRICA         = 0x10,     // Sub-Saharan Africa
    ZONE_AMERICA_LATIN  = 0x20,     // Latin America and the Caribbean
    ZONE_AMERICA_NORTH  = 0x30,     // North America
    ZONE_ASIA_EAST      = 0x40,     // East Asia and Pacific
    ZONE_ASIA_SOUTH     = 0x50,     // South Asia
    ZONE_EUROPE         = 0x60,     // Europe and Central Asia
    ZONE_MIDDLEEAST     = 0x70,     // Middle East and North Africa
};

enum featDeclarationType {
    // For self-declarations.
    FTYPE_STUDENT = 0x100,

    // Only schools themselves.
    FTYPE_SCHOOL = 0x200,

    // A transportation provide such as a bus driver.
    FTYPE_SERVPROV = 0x300,

    // A teacher, coach, or similar person in the direct employ of a school.
    FTYPE_VERIFIER = 0x400,

    // A police officer, judicial personnel, respected community member, or
    // other person with credentials and responsibilities.
    FTYPE_INDEPENDENT = 0x500,
};

// These are special-purpose flags. Any that are not noted in the documentation
// should be considered private, and support for them may change or be
// discontinued at any time.
enum otherFlags {
    FLAG_RAWPUBLICKEY   = 0x01000000,
    FLAG_ADDIFNEEDED    = 0x02000000,
};

#define ADDRESS_BUFFERLENGTH 64         // less than 42 characters at present
#define VALUE_BUFFERLENGTH 32           // 64-bit in decimal is only 20 characters
#define MEMO_BUFFERLENGTH 256
#define IDENTIFIER_BUFFERLENGTH 128     // less than 80 characters at present

struct batchtransfer_data {
    char toAddress[ADDRESS_BUFFERLENGTH];
    char valueInToshi[VALUE_BUFFERLENGTH];
    char memo[MEMO_BUFFERLENGTH];

    // Output parameter.
    char identifier[IDENTIFIER_BUFFERLENGTH];
};

// These functions should not be decorated.
#ifdef __cplusplus
extern "C" {
#endif

// These are presently undocumented.
int setLoggingFilePointer(FILE *fp);
int setLoggingFileDescriptor(int fd);
reqtoken runRbbcTest(sessiontoken t, unsigned int count);
reqtoken nodesOnline(sessiontoken t, int zone, unsigned startIndex);
reqtoken submitBarePoeFeats(sessiontoken t, unsigned int zone_and_network,
    const char *featDeclaration);

const char* walletLibraryVersion();

const char* createNewWallet(const char *reserved, const char *passphrase);
sessiontoken createSession(const char *cacheBytes, const char *walletBytes,
    const char *passphrase);
void freeSession(sessiontoken t);

const char* getAddress(sessiontoken t, unsigned int addressNumber, unsigned int
    network);
const char* getCacheBytes(sessiontoken t);
const char* getWalletBytes(sessiontoken t, const char *passphrase);

const char* calculateTransferFee(const char *recipientReceivesInToshi);
const char* separateTransferAndFee(const char *totalInToshi);
reqtoken getBalance(sessiontoken t, const char *address);
reqtoken getHistoryTransactions(sessiontoken t, const char *address, long start);
reqtoken getWalletBalance(sessiontoken t, unsigned int addressNumber, unsigned
    int network);
reqtoken submitAdminInfo(sessiontoken tt, unsigned int network, const char
    *commandDeclarations);
reqtoken submitPoeFeats(sessiontoken t, unsigned int zone_and_network,
    unsigned int featDeclarationType, const char *twoLetterCountryCode,
    const char *dateBeingDeclared, const char *childWalletIds);
reqtoken submitTransfer(sessiontoken t, unsigned int addressNumber,
    const char *toAddress, const char *valueInToshi, const char *feeInToshi,
    const char *memo);
reqtoken submitMultiTransfer(sessiontoken t, unsigned int timeoutSeconds,
    unsigned int addressNumber, struct batchtransfer_data *data,
    unsigned int datacount);
reqtoken submitBatchTransfer(sessiontoken t, unsigned int timeoutSeconds,
    unsigned int addressNumber, struct batchtransfer_data *data,
    unsigned int datacount);
reqtoken transfer(sessiontoken t, unsigned int addressNumber,
    const char *toAddress, const char *valueInToshi, const char *feeInToshi,
    const char *memo); // DEPRECATED in favor of submitTransfer().
const char* getRequestResult(reqtoken t, unsigned int n);
unsigned int getRequestResultCount(reqtoken t);
int getRequestStatus(reqtoken t);
void freeRequest(reqtoken t);

reqtoken getBlockByIndex(sessiontoken t, unsigned int zone_network, unsigned
    int index, int includeTransactionDetails);
reqtoken getBlockByTime(sessiontoken t, unsigned int zone_network, unsigned int
    timestamp, int includeTransactionDetails);
reqtoken getBlockByIdentifier(sessiontoken t, const char *id, int
    includeTransactionDetails);
reqtoken getLastBlock(sessiontoken t, unsigned int network, int
    includeTransactionDetails);
reqtoken getTransaction(sessiontoken t, const char *id);

reqtoken getActiveNodeCount(sessiontoken t);
reqtoken getNodeStatus(sessiontoken t, const char *nodeName);

const char* getFoundationBytes(sessiontoken t);
int validateSessionToken(sessiontoken t);
int validateRequestToken(reqtoken t);
int verifyWalletBytes(const char *walletBytes, const char *passphrase);

const char* multisigCreate(sessiontoken t, const char *fromAddress,
    const char *toAddress, const char *valueInToshi, const char *feeInToshi,
    const char *memo);
const char *multisigData(sessiontoken t, const char *rawdata);
const char *multisigSign(sessiontoken t, const char *rawdata);
reqtoken multisigSubmit(sessiontoken t, const char *rawdata);

#ifdef __cplusplus
}
#endif

// This is used by the testing system, you shouldn't need it for anything else.
#ifdef __cplusplus
    #include <string>
    #include <vector>

    void _setDefaultServers(const std::vector<std::string> &list);
#endif // __cplusplus

// A set of C++-specific wrappers, to simplify using the library with that
// language. Note that these types are NOT thread-safe.
//
// NOTE: this C++ code should be considered BETA. It *should* simply reflect
// the C API with C++ simplifications, but there may be some divergence until
// the C++ code is finalized.
#ifdef __cplusplus

#include <cstdint>
#include <fstream>
#include <sstream>
#include <memory>
#include <thread>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// The three C++ wrappers representing the three denominations of ShaktiCoin.
// They are interchangeable, the only reason to use different ones is to
// initialize a value in a specific denomination, or to get a value represented
// as a specific denomination.
namespace Shakti {

const uint64_t cChaiPerCoin = 100;
const uint64_t cToshiPerChai = 1000;
const uint64_t cToshiPerCoin = cChaiPerCoin * cToshiPerChai;
const uint64_t cMaxToshi = UINT64_C(0xffffffffffffffff);
const uint64_t cMaxChai = cMaxToshi / cToshiPerChai;
const uint64_t cMaxCoin = cMaxToshi / cToshiPerCoin;

// The base currency type, for the shared functions.
class ValueBase {
    public:
    explicit ValueBase(uint64_t toshi): mToshi(toshi) { }

    uint64_t valueInToshi() const { return mToshi; }

    explicit operator bool() const { return (mToshi != 0); }

    int compare(const ValueBase &v) const {
        if (mToshi == v.mToshi)
            return 0;
        else
            return (mToshi < v.mToshi ? -1 : 1);
    }
    bool operator==(const ValueBase &v) const { return (compare(v) == 0); }
    bool operator!=(const ValueBase &v) const { return (compare(v) != 0); }
    bool operator<(const ValueBase &v) const { return (compare(v) < 0); }
    bool operator<=(const ValueBase &v) const { return (compare(v) <= 0); }
    bool operator>(const ValueBase &v) const { return (compare(v) > 0); }
    bool operator>=(const ValueBase &v) const { return (compare(v) >= 0); }

    template <typename T>
    T sum(const ValueBase &v) const {
        uint64_t s = mToshi + v.mToshi;
        if (s < mToshi || s < v.mToshi)
            throw std::overflow_error("sum overflowed internal type");
        return T(ValueBase(s));
    }

    template <typename T>
    T diff(const ValueBase &v) const {
        if (mToshi < v.mToshi)
            throw std::underflow_error("can't represent values less than zero");
        return T(ValueBase(mToshi - v.mToshi));
    }

    template <typename T>
    T& add(const ValueBase &v) {
        uint64_t s = mToshi + v.mToshi;
        if (s < mToshi || s < v.mToshi)
            throw std::overflow_error("sum overflowed internal type");
        mToshi = s;
        return (T&)(*this);
    }

    template <typename T>
    T& sub(const ValueBase &v) {
        if (mToshi < v.mToshi)
            throw std::underflow_error("can't represent values less than zero");
        mToshi -= v.mToshi;
        return (T&)(*this);
    }

    protected:
    uint64_t mToshi = 0;
};

// Whole coins.
class Coin: public ValueBase {
    public:
    explicit Coin(uint64_t coins, uint64_t chai = 0, uint64_t toshi = 0):
        ValueBase(coins * cToshiPerCoin)
    {
        try {
            if (coins > cMaxCoin || chai > cMaxChai)
                throw std::invalid_argument("value too large");
            if (chai) add<Coin>(ValueBase(chai * cToshiPerChai));
            if (toshi) add<Coin>(ValueBase(toshi));
        } catch (std::overflow_error&) {
            throw std::invalid_argument("value too large");
        }
    }
    Coin(const ValueBase &value): ValueBase(value.valueInToshi()) { }

    Coin operator+(const ValueBase &v) const { return sum<Coin>(v); }
    Coin& operator+=(const ValueBase &v) { return add<Coin>(v); }
    Coin operator-(const ValueBase &v) const { return diff<Coin>(v); }
    Coin& operator-=(const ValueBase &v) { return sub<Coin>(v); }

    uint64_t value() const { return mToshi / cToshiPerCoin; }
};

// 1/100th of a coin, the "cents" of the currency.
class Chai: public ValueBase {
    public:
    explicit Chai(uint64_t chai): ValueBase(chai * cToshiPerChai) {
        if (chai > cMaxChai)
            throw std::invalid_argument("value too large");
    }
    Chai(const ValueBase &value): ValueBase(value.valueInToshi()) { }

    Chai operator+(const ValueBase &v) const { return sum<Chai>(v); }
    Chai& operator+=(const ValueBase &v) { return add<Chai>(v); }
    Chai operator-(const ValueBase &v) const { return diff<Chai>(v); }
    Chai& operator-=(const ValueBase &v) { return sub<Chai>(v); }

    uint64_t value() const { return mToshi / cToshiPerChai; }
};

// 1/100,000th of a coin, the smallest denomination possible.
class Toshi: public ValueBase {
    public:
    explicit Toshi(uint64_t toshi): ValueBase(toshi) { }
    explicit Toshi(const std::string &value): ValueBase(0) {
        std::istringstream in(value);
        in >> mToshi;
        if (!in.fail()) {
            in >> std::ws;
            if (in.get() == EOF) return;
        }
        throw std::invalid_argument("could not interpret string as toshi");
    }
    Toshi(const ValueBase &value): ValueBase(value.valueInToshi()) { }

    Toshi operator+(const ValueBase &v) const { return sum<Toshi>(v); }
    Toshi& operator+=(const ValueBase &v) { return add<Toshi>(v); }
    Toshi operator-(const ValueBase &v) const { return diff<Toshi>(v); }
    Toshi& operator-=(const ValueBase &v) { return sub<Toshi>(v); }

    uint64_t value() const { return mToshi; }
    std::string str() const { std::ostringstream out; out << mToshi; return
        out.str(); }
};

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>
    &out, const ValueBase &value)
{
    auto vtoshi = value.valueInToshi();
    auto coin = vtoshi / cToshiPerCoin;
    out << coin;

    std::ostringstream tmp;
    tmp << (vtoshi % cToshiPerCoin);
    auto toshistr = tmp.str();
    while (!toshistr.empty() && toshistr[toshistr.length() - 1] == '0')
        toshistr = toshistr.substr(0, toshistr.length() - 1);
    if (!toshistr.empty()) out << '.' << toshistr;
    return out;
}

} // namespace Shakti

namespace walletlib {

// The C API returns a `NULL char*` for some calls, and the C++ `std::string`
// type can't accept a `NULL`, so we created this class. It's conceptually
// identical to `std::optional<std::string>`, but `std::optional` isn't
// available until C++14, and this code targets C++11.
class optstring {
    public:
    optstring(const std::nullptr_t& = nullptr) { }
    optstring(const std::string &str): mString(str), mSet(true) { }
    optstring(const char *str): mString(str == nullptr ? std::string() :
        std::string(str)), mSet(str != nullptr) { }

    optstring& operator=(const std::nullptr_t&) { mString.clear(); mSet =
        false; return *this; }
    optstring& operator=(const std::string &s) { mString = s; mSet = true;
        return *this; }
    optstring& operator=(const char *s) { if (s) { mString = s; mSet = true; }
        else { mString.clear(); mSet = false; } return *this; }

    explicit operator bool() const { return mSet; }
    const std::string& operator*() const {
        if (mSet) return mString;
        throw std::runtime_error("attempted to dereference unset optstring");
    }
    const std::string* operator->() const {
        if (mSet) return &mString;
        throw std::runtime_error("attempted to dereference unset optstring");
    }

    const char* raw() const { if (mSet) return mString.c_str(); else return
        nullptr; }

    private:
    std::string mString;
    bool mSet = false;
};

////////////////////////////////////////////////////////////////////////////////
// A C++ substitute for the batchtransfer_data struct. You can subclass the
// nested UserData class if desired.
struct BatchTransfer {
    struct UserData {
        virtual ~UserData() { }
    };

    BatchTransfer(): value(0) { }

    BatchTransfer(const std::string &_toAddress,
        const Shakti::Coin &_value,
        const std::string &_memo = { })
    :
        toAddress(_toAddress),
        value(_value),
        memo(_memo)
    {
        // Nothing needed here.
    }

    // Data needed for the transfer.
    std::string toAddress;
    Shakti::Coin value;
    std::string memo;

    // Available for whatever the caller deigns to use it for.
    std::shared_ptr<UserData> userdata;

    // Output parameter.
    std::string identifier;
};

////////////////////////////////////////////////////////////////////////////////
// A C++ wrapper for the reqtoken type.
class Request {
    public:
    // The main constructors.
    //
    // Most calls are complete when `getRequestStatus()` returns its first
    // positive value, but some return a second positive value before they're
    // finished. Thus the constructor that takes a `completionValue` parameter.
    explicit Request(reqtoken t = nullptr): mData(t ?
        std::make_shared<_data_t>(t) : nullptr) { }
    Request(reqtoken t, int completionValue): mData(t ?
        std::make_shared<_data_t>(t, completionValue) : nullptr) { }

    // Retrieve the status of the request. Zero means it's still running; a
    // positive value means that it was successful (but might not be complete,
    // depending on the call); a negative value means some sort of error.
    //
    // By default, this function will return with the current status
    // immediately. If the `wait` parameter is set to true or 1, it won't
    // return until the status is non-zero. If it's set to anything greater
    // than 1, it won't return until the status has reached its "completed"
    // value (set in the constructor).
    int status(unsigned wait = false) { return (mData ? mData->status(wait) :
        -1000000); }

    // Retrieve a specific result. You'll generally want to wait until status()
    // returns a non-zero value, but some calls provide useful results
    // immediately.
    optstring result(unsigned index = 0) {
        return mData ? getRequestResult(mData->mToken, index) : nullptr;
    }

    // Returns the number of results currently available for the request.
    unsigned count() { return mData ? getRequestResultCount(mData->mToken) : 0; }

    private: ///////////////////////////////////////////////////////////////////
    struct _data_t {
        _data_t(reqtoken t, int comp = 1): mToken(t), mComplete(comp) { }
        ~_data_t() { freeRequest(mToken); }

        int status(unsigned wait) {
            if (mStatus >= 0 && mStatus != mComplete) {
                while (true) {
                    mStatus = getRequestStatus(mToken);
                    if (mStatus >= 0 &&
                        ((wait > 1 && mStatus != mComplete) ||
                        (wait == 1 && mStatus == 0)))
                    {
                        // Wait a bit, then loop back and try again.
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    } else {
                        // Return the latest status immediately.
                        break;
                    }
                }
            }
            return mStatus;
        }

        reqtoken mToken = nullptr;
        int mStatus = 0;
        int mComplete = 1;
    };

    std::shared_ptr<_data_t> mData;
};

////////////////////////////////////////////////////////////////////////////////
// A C++ wrapper for the sessiontoken type.
class Session {
    public:
    explicit Session(sessiontoken t = nullptr):
        mData(std::make_shared<_data_t>(t)) { }

    Session(const optstring &cacheBytes, const optstring &walletBytes, const
        optstring &passphrase = nullptr)
    {
        auto t = createSession(cacheBytes.raw(), walletBytes.raw(),
            passphrase.raw());
        if (!t) throw std::runtime_error("unable to create session; passphrase "
            "is probably wrong");
        mData = std::make_shared<_data_t>(t);
    }

    // Returns true so long as the session is open.
    explicit operator bool() const { return (mData.get() != nullptr &&
        mData->mToken != nullptr); }

    // Closes the session. This is done automatically when the object is
    // destroyed.
    void close() { mData.reset(); }

    optstring getCacheBytes() { return ::getCacheBytes(mData->mToken); }
    optstring getFoundationBytes() { return ::getFoundationBytes(mData->mToken); }
    optstring getWalletBytes(const optstring &passphrase) { return
        ::getWalletBytes(mData->mToken, passphrase.raw()); }



    // inTestnet can usually be treated as a Boolean, unless you need extra
    // flags for it.
    optstring getAddress(unsigned addressNumber, unsigned int inTestnet = 0) {
        return ::getAddress(mData->mToken, addressNumber, inTestnet);
    }

    Request getBalance(const std::string &address) {
        return Request(::getBalance(mData->mToken, address.c_str()));
    }

    Request getBlockByIndex(unsigned zone_network, unsigned index, bool
        includeTransactionDetails)
    {
        return Request(::getBlockByIndex(mData->mToken, zone_network, index,
            includeTransactionDetails));
    }

    Request getBlockByIdentifier(const std::string &identifier, bool
        includeTransactionDetails)
    {
        return Request(::getBlockByIdentifier(mData->mToken, identifier.c_str(),
            includeTransactionDetails));
    }

    Request getBlockByTime(unsigned zone_network, time_t timestamp, bool
        includeTransactionDetails)
    {
        return Request(::getBlockByTime(mData->mToken, zone_network,
            unsigned(timestamp), includeTransactionDetails));
    }

    Request getHistoryTransactions(const std::string &address, long start) {
        return Request(::getHistoryTransactions(mData->mToken, address.c_str(),
            start));
    }

    Request getLastBlock(unsigned zone_network, bool includeTransactionDetails) {
        return Request(::getLastBlock(mData->mToken, zone_network,
            includeTransactionDetails));
    }

    Request getTransaction(const std::string &identifier) {
        return Request(::getTransaction(mData->mToken, identifier.c_str()));
    }

    Request getWalletBalance(unsigned addressNumber, bool inTestnet = false) {
        return Request(::getWalletBalance(mData->mToken, addressNumber,
            inTestnet));
    }

    Request nodesOnline(int zone, unsigned startIndex = 0) {
        return Request(::nodesOnline(mData->mToken, zone, startIndex));
    }

    Request getActiveNodeCount() {
        return Request(::getActiveNodeCount(mData->mToken));
    }

    // In practice, this should take an std::string instead of an optstring.
    // The optstring is only here to allow the system to test that a nullptr is
    // properly handled.
    Request getNodeStatus(const optstring &nodeName) {
        return Request(::getNodeStatus(mData->mToken, nodeName.raw()));
    }

    Request submitPoeFeats(unsigned zone_network, unsigned featDeclType,
        const std::string &twoLetterCountryCode,
        const std::string &dateBeingDeclared,
        const std::string &childWalletIds)
    {
        return Request(::submitPoeFeats(mData->mToken, zone_network,
            featDeclType, twoLetterCountryCode.c_str(),
            dateBeingDeclared.c_str(), childWalletIds.c_str()));
    }

    Request submitPoeFeats(unsigned zone_network, const std::string
        &featDeclarations)
    {
        return Request(::submitBarePoeFeats(mData->mToken, zone_network,
            featDeclarations.c_str()));
    }

    Request submitAdminInfo(unsigned int network, const std::string &decls) {
        return Request(::submitAdminInfo(mData->mToken, network, decls.c_str()),
            2);
    }

    // DEPRECATED. Use the otherwise-identical submitTransfer() instead.
    Request transfer(unsigned fromAddressNumber, const std::string &toAddress,
        const Shakti::Coin &value, const std::string &memo = { })
    {
        return submitTransfer(fromAddressNumber, toAddress, value, memo);
    }

    // DEPRECATED. Use the otherwise-identical submitTransfer() instead.
    Request transfer(unsigned fromAddressNumber, const std::string &toAddress,
        const Shakti::Coin &value, const Shakti::Coin &fee,
        const std::string &memo = { })
    {
        return submitTransfer(fromAddressNumber, toAddress, value, fee, memo);
    }

    // This version lets the library calculate the fee.
    Request submitTransfer(unsigned fromAddressNumber,
        const std::string &toAddress, const Shakti::Coin &value,
        const std::string &memo = { })
    {
        return Request(::submitTransfer(mData->mToken, fromAddressNumber,
            toAddress.c_str(), Shakti::Toshi(value).str().c_str(),
            nullptr, memo.c_str()), 2);
    }

    Request submitTransfer(unsigned fromAddressNumber,
        const std::string &toAddress, const Shakti::Coin &value,
        const Shakti::Coin &fee, const std::string &memo = { })
    {
        return Request(::submitTransfer(mData->mToken, fromAddressNumber,
            toAddress.c_str(), Shakti::Toshi(value).str().c_str(),
            Shakti::Toshi(fee).str().c_str(), memo.c_str()), 2);
    }

    Request submitMultiTransfer(unsigned timeoutSeconds,
        unsigned fromAddressNumber,
        std::vector<BatchTransfer> &data)
    {
        return submitBatchTransfer(timeoutSeconds, fromAddressNumber, data);
    }

    Request submitBatchTransfer(unsigned timeoutSeconds,
        unsigned fromAddressNumber,
        std::vector<BatchTransfer> &data)
    {
        struct xferbuf {
            xferbuf(std::vector<BatchTransfer> &data):
                mTransferData(data)
            {
                // Make sure all of the data is valid, so we get any exceptions
                // out of the way before we do any allocations.
                for (auto &d: mTransferData) {
                    if (d.toAddress.length() >= ADDRESS_BUFFERLENGTH)
                        throw std::runtime_error("toAddress too long");
                    if (d.memo.length() >= MEMO_BUFFERLENGTH)
                        throw std::runtime_error("memo too long");
                }

                mBuffer = new batchtransfer_data[mTransferData.size()];
                batchtransfer_data *p = mBuffer;
                for (auto &d: mTransferData) {
                    strcpy(p->toAddress, d.toAddress.c_str());
                    strcpy(p->valueInToshi, Shakti::Toshi(d.value).str().c_str());
                    strcpy(p->memo, d.memo.c_str());
                    ++p;
                }
            }

            ~xferbuf() { delete[] mBuffer; }

            void copyIdentifiers() {
                batchtransfer_data *p = mBuffer;
                for (auto &d: mTransferData) {
                    d.identifier = p->identifier;
                    ++p;
                }
            }

            std::vector<BatchTransfer> &mTransferData;
            batchtransfer_data *mBuffer = nullptr;
        };

        xferbuf buffer(data);
        Request rval(::submitBatchTransfer(mData->mToken, timeoutSeconds,
            fromAddressNumber, buffer.mBuffer,
            unsigned(buffer.mTransferData.size())));
        buffer.copyIdentifiers();
        return rval;
    }

    optstring multisigCreate(const std::string &fromAddress,
        const std::string &toAddress, const Shakti::Coin &value,
        const Shakti::Coin &fee, const std::string &memo = { })
    {
        return ::multisigCreate(mData->mToken, fromAddress.c_str(),
            toAddress.c_str(), Shakti::Toshi(value).str().c_str(),
            Shakti::Toshi(fee).str().c_str(), memo.c_str());
    }

    optstring multisigCreate(const std::string &fromAddress,
        const std::string &toAddress, const Shakti::Coin &value,
        const std::string &memo = { })
    {
        return ::multisigCreate(mData->mToken, fromAddress.c_str(),
            toAddress.c_str(), Shakti::Toshi(value).str().c_str(),
            nullptr, memo.c_str());
    }

    optstring multisigData(const std::string &rawdata) {
        return ::multisigData(mData->mToken, rawdata.c_str());
    }

    optstring multisigSign(const std::string &rawdata) {
        return ::multisigSign(mData->mToken, rawdata.c_str());
    }

    Request multisigSubmit(const std::string &rawdata) {
        return Request(::multisigSubmit(mData->mToken, rawdata.c_str()), 2);
    }

    private: ///////////////////////////////////////////////////////////////////
    struct _data_t {
        _data_t(sessiontoken t): mToken(t) { }
        ~_data_t() { freeSession(mToken); }

        sessiontoken mToken;
    };

    std::shared_ptr<_data_t> mData;
};

////////////////////////////////////////////////////////////////////////////////
// Stand-alone functions

// Reads in the contents of a file, or throws an exception if it can't. Can be
// useful for the Session constructor's `cacheBytes` and `walletBytes`
// parameters.
inline std::string contentsOf(const std::string &filename) {
    std::ifstream in(filename);
    if (in) {
        const std::streamsize cBufferSize = 256;
        std::string rval;
        std::vector<char> buffer(cBufferSize);
        while (true) {
            in.read(&buffer[0], cBufferSize);
            auto bytesread = in.gcount();
            rval += std::string(&buffer[0], bytesread);
            if (bytesread < cBufferSize) break;
        }
        return rval;
    } else throw std::runtime_error("failed to open file '" + filename + "'");
}

inline optstring createNewWallet(const optstring &passphrase) {
    return ::createNewWallet(nullptr, passphrase.raw());
}

inline bool verifyWalletBytes(const std::string &walletBytes, const optstring
    &passphrase)
{
    return ::verifyWalletBytes(walletBytes.c_str(), passphrase.raw());
}

inline Shakti::Coin calculateTransferFee(const Shakti::Coin &recipientReceives) {
    optstring rval = ::calculateTransferFee(Shakti::Toshi(
        recipientReceives).str().c_str());
    if (!rval) throw std::invalid_argument("invalid transfer amount");
    return Shakti::Toshi(*rval);
}

inline std::pair<Shakti::Coin,Shakti::Coin> separateTransferAndFee(const
    Shakti::Coin &total)
{
    optstring rval = ::separateTransferAndFee(Shakti::Toshi(total).str().c_str());
    if (!rval) throw std::invalid_argument("unable to separate transfer and fee");

    // The returned value is two numbers separated by a comma.
    std::istringstream in(*rval);
    std::string value;
    std::getline(in, value, ',');
    Shakti::Toshi recipientReceives(value);
    std::getline(in, value, ',');
    Shakti::Toshi fee(value);
    return std::make_pair(recipientReceives, fee);
}

inline std::string version() {
    return ::walletLibraryVersion();
}

} // namespace walletlib
#endif // __cplusplus

#endif // INCLUDEGUARD_4ce04acb_5791_4e27_b3ca_1fca8006ee0f

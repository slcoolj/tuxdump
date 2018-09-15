#pragma once
#include "tprocess/memory.h"
#include "tprocess/region.h"
#include <cstdint>
#include <cstdio>
#include <vector>

struct ClientClass {
    uintptr_t m_pCreateFn;
    uintptr_t m_pCreateEventF;
    uintptr_t m_pNetworkName;
    uintptr_t m_pRecvTable;
    uintptr_t m_pNext;
    int m_ClassID;
};

enum class SendPropType : int {
    DPT_Int = 0,
    DPT_Float,
    DPT_Vector,
    DPT_VectorXY,
    DPT_String,
    DPT_Array,
    DPT_DataTable,
    DPT_Int64,
    DPT_NUMSendPropTypes
};

struct RecvProp {
    uintptr_t m_pVarName; // const char*
    SendPropType m_RecvType;
    int m_Flags;
    int m_StringBufferSize;
    bool m_bInsideArray;
    uint8_t pad0[3];
    uintptr_t m_pExtraData;
    uintptr_t m_pArrayProp;
    uintptr_t m_ArrayLengthProxy;
    uintptr_t m_ProxyFn;
    uintptr_t m_DataTableProxyFn;
    uintptr_t m_pDataTable;
    int m_Offset;
    int m_ElementStride;
    int m_nElements;
    uint8_t pad1[4];
    uintptr_t m_pParentArrayPropName;
};

struct RecvTable {
    uintptr_t m_pProps;
    int m_nProps;
    uint8_t pad0[4];
    uintptr_t m_pDecoder;
    uintptr_t m_pNetTableName;
    bool m_bInitialized;
    bool m_bInMainList;
};

enum class NetVarOutputStyle {
    CPlusPlus,
    Raw
};

class NetVarManager : public TProcess::Memory {
    struct NetVar_Prop {
        std::string name;
        SendPropType type;
        int stringSize = 0;
        size_t offset;
    };
    struct NetVar_Table {
        std::string name;
        std::string typeString;
        size_t propSize = 0;
        NetVar_Prop prop;
        size_t offset;
        size_t size;
        std::vector<NetVar_Prop> child_props;
        std::vector<NetVar_Table> child_tables;
    };
    public:
        NetVarManager(uintptr_t addr);
        ~NetVarManager();
        void Dump(NetVarOutputStyle style = NetVarOutputStyle::Raw, bool comments = false);
        void DumpTableCPP(const NetVar_Table& table, size_t indent = 2);
        void DumpTableRaw(const NetVar_Table& table, size_t indent = 1);
    private:
        void PrintIndent(size_t indents = 1);
        const char* PropToString(const NetVar_Prop& prop);
        std::string TableToString(const NetVar_Table& table);
        NetVar_Table LoadTable(RecvTable& recvTable);
    private:
        std::vector<NetVar_Table> m_data;
        FILE* m_dumpFile = nullptr;
        bool m_useComments = false;
};

#pragma once


#include "types.h"
#include <stdexcept>
#include <string>
#include <vector>

#ifdef __cpp_impl_three_way_comparison
#include <compare>
#endif


#define DECLARE_BASE_CLASS(class_name) \
protected: \
    static HSS_Types::RuntimeClass* _GetBaseClass() noexcept; \
public: \
    static const HSS_Types::RuntimeClass class##class_name; \
    static HSS_Types::RuntimeClass const* GetThisClass() noexcept; \
    virtual HSS_Types::RuntimeClass const* GetRuntimeClass() const noexcept

#define DECLARE_BASE_CLASS_CREATE(class_name) \
    DECLARE_BASE_CLASS(class_name); \
    static HSS_Types::BaseObject* CreateObject()

#define _RUNTIME_BASE_CLASS(class_name) dynamic_cast<HSS_Types::RuntimeClass const*>(&class_name::class##class_name)

#define _RUNTIME_BASE_CLASS_NAMESPACE_ROOT(nmspace, class_name) dynamic_cast<HSS_Types::RuntimeClass const*>(&nmspace::class_name::class##class_name)

#define _IMPLEMENT_BASE_CLASS_ROOT(class_name, constructor) \
    HSS_Types::RuntimeClass* class_name::_GetBaseClass() noexcept { return nullptr; } \
    const HSS_Types::RuntimeClass class_name::class##class_name(#class_name, constructor, class_name::_GetBaseClass); \
    HSS_Types::RuntimeClass const* class_name::GetThisClass() noexcept { return _RUNTIME_BASE_CLASS(class_name); } \
    HSS_Types::RuntimeClass const* class_name::GetRuntimeClass() const noexcept { return _RUNTIME_BASE_CLASS(class_name); }

#define IMPLEMENT_BASE_CLASS_ROOT(class_name) \
    _IMPLEMENT_BASE_CLASS_ROOT(class_name, nullptr)

#define _IMPLEMENT_BASE_CLASS(class_name, base_class_name, constructor) \
    HSS_Types::RuntimeClass* class_name::_GetBaseClass() noexcept { return _RUNTIME_BASE_CLASS(base_class_name); } \
    const HSS_Types::RuntimeClass class_name::class##class_name(#class_name, constructor, class_name::_GetBaseClass); \
    HSS_Types::RuntimeClass const* class_name::GetThisClass() noexcept { return _RUNTIME_BASE_CLASS(class_name); } \
    HSS_Types::RuntimeClass const* class_name::GetRuntimeClass() const noexcept { return _RUNTIME_BASE_CLASS(class_name); }

#define IMPLEMENT_BASE_CLASS(class_name, base_class_name) \
    _IMPLEMENT_BASE_CLASS(class_name, base_class_name, nullptr)

#define _IMPLEMENT_BASE_CLASS_NAMESPACE_ROOT(nmspace, class_name, constructor) \
    HSS_Types::RuntimeClass* nmspace::class_name::_GetBaseClass() noexcept { return nullptr; } \
    const HSS_Types::RuntimeClass nmspace::class_name::class##class_name(#nmspace "::" #class_name, constructor, class_name::_GetBaseClass); \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetThisClass() noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); } \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetRuntimeClass() const noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); }

#define IMPLEMENT_BASE_CLASS_NAMESPACE_ROOT(nmspace, class_name) \
    _IMPLEMENT_BASE_CLASS_NAMESPACE_ROOT(nmspace, class_name, nullptr)

#define _IMPLEMENT_BASE_CLASS_NAMESPACE(nmspace, class_name, base_class_name, constructor) \
    HSS_Types::RuntimeClass* nmspace::class_name::_GetBaseClass() noexcept { return _RUNTIME_BASE_CLASS(base_class_name); } \
    const HSS_Types::RuntimeClass nmspace::class_name::class##class_name(#nmspace "::" #class_name, constructor, class_name::_GetBaseClass); \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetThisClass() noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); } \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetRuntimeClass() const noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); }

#define IMPLEMENT_BASE_CLASS_NAMESPACE(nmspace, class_name, base_class_name) \
    _IMPLEMENT_BASE_CLASS_NAMESPACE(nmspace, class_name, base_class_name, nullptr)

#define _IMPLEMENT_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name, constructor) \
    HSS_Types::RuntimeClass* nmspace::class_name::_GetBaseClass() noexcept { return _RUNTIME_BASE_CLASS(base_nmspace, base_class_name); } \
    const HSS_Types::RuntimeClass nmspace::class_name::class##class_name(#nmspace "::" #class_name, constructor, nmspace::class_name::_GetBaseClass); \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetThisClass() noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); } \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetRuntimeClass() const noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); }

#define IMPLEMENT_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name) \
    _IMPLEMENT_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name, nullptr)

#define _IMPLEMENT_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name, constructor) \
    HSS_Types::RuntimeClass* nmspace::class_name::_GetBaseClass() noexcept { return _RUNTIME_BASE_CLASS(base_nmspace, base_class_name); } \
    const HSS_Types::RuntimeClass nmspace::class_name::class##class_name(#nmspace "::" #class_name, constructor, nmspace::class_name::_GetBaseClass); \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetThisClass() noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); } \
    HSS_Types::RuntimeClass const* nmspace::class_name::GetRuntimeClass() const noexcept { return _RUNTIME_BASE_CLASS(nmspace, class_name); }

#define IMPLEMENT_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name) \
    _IMPLEMENT_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name, nullptr)

#define IMPLEMENT_CREATE_BASE_CLASS_ROOT(class_name) \
    _IMPLEMENT_BASE_CLASS_ROOT(class_name, class_name::CreateObject)

#define IMPLEMENT_CREATE_BASE_CLASS(class_name, base_class_name) \
    _IMPLEMENT_BASE_CLASS(class_name, base_class_name, class_name::CreateObject)

#define IMPLEMENT_CREATE_BASE_CLASS_NAMESPACE_ROOT(nmspace, class_name) \
    _IMPLEMENT_BASE_CLASS_NAMESPACE_ROOT(nmspace, class_name, nmspace::class_name::CreateObject)

#define IMPLEMENT_CREATE_BASE_CLASS_NAMESPACE(nmspace, class_name, base_class_name) \
    _IMPLEMENT_BASE_CLASS_NAMESPACE(nmspace, class_name, base_class_name, nmspace::class_name::CreateObject)

#define IMPLEMENT_CREATE_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name) \
    _IMPLEMENT_BASE_CLASS_2NAMESPACE(nmspace, class_name, base_nmspace, base_class_name, nmspace::class_name::CreateObject)

namespace HSS_Types
{
    class RuntimeClass;
    class BaseObject
    {
    public:
        static RuntimeClass* _GetBaseClass() noexcept { return nullptr; }
        static RuntimeClass const* GetThisClass() noexcept { return nullptr; }
    };

    typedef std::vector<HSS_Types::BaseObject*> BaseObArray;


    class RuntimeClass
    {
    private:
        std::string m_name;

    public:
        typedef HSS_Types::BaseObject* (*ConstructorFunc)();
        typedef RuntimeClass* (*GetBaseClassFunc)();

    private:
        ConstructorFunc m_constructor;
        GetBaseClassFunc m_baseClass;

    public:
        RuntimeClass(const std::string name, ConstructorFunc constructor) : m_name(name), m_constructor(constructor), m_baseClass(nullptr) { }
        RuntimeClass(const std::string name, ConstructorFunc constructor, GetBaseClassFunc baseClass) : m_name(name), m_constructor(constructor), m_baseClass(baseClass) { }
        RuntimeClass(const RuntimeClass& other) : m_name(other.m_name), m_constructor(other.m_constructor), m_baseClass(other.m_baseClass) { }
        RuntimeClass(RuntimeClass&& other) noexcept : m_name(std::move(other.m_name)), m_constructor(std::move(other.m_constructor)), m_baseClass(std::move(other.m_baseClass)) { }
        virtual ~RuntimeClass() { }
        RuntimeClass& operator=(const RuntimeClass& other) = delete;
        RuntimeClass& operator=(RuntimeClass&& other) = delete;

        HSS_Types::BaseObject* CreateObject() noexcept { if (m_constructor) return m_constructor(); return nullptr; }
        RuntimeClass* GetBaseClass() noexcept { if (m_baseClass) return m_baseClass(); return nullptr; }
        std::string GetClassName() const { return m_name; }

        bool IsDerivedFrom(const RuntimeClass* other) noexcept { RuntimeClass* cls = GetBaseClass(); while (cls) { if (cls == other) return true; cls = cls->GetBaseClass(); } return false; }

        static inline int cmp(const RuntimeClass& lhs, const RuntimeClass& rhs) noexcept { return lhs.m_name.compare(rhs.m_name); }

#ifdef __cpp_impl_three_way_comparison
        std::weak_ordering operator<=>(const RuntimeClass& other) const noexcept { return cmp(*this, other) <=> 0; }
#else
        friend bool operator==(const RuntimeClass& lhs, const RuntimeClass& rhs) noexcept { return cmp(lhs, rhs) == 0; }
        friend bool operator!=(const RuntimeClass& lhs, const RuntimeClass& rhs) noexcept { return cmp(lhs, rhs) != 0; }
        friend bool operator< (const RuntimeClass& lhs, const RuntimeClass& rhs) noexcept { return cmp(lhs, rhs) < 0; }
        friend bool operator> (const RuntimeClass& lhs, const RuntimeClass& rhs) noexcept { return cmp(lhs, rhs) > 0; }
        friend bool operator<=(const RuntimeClass& lhs, const RuntimeClass& rhs) noexcept { return cmp(lhs, rhs) <= 0; }
        friend bool operator>=(const RuntimeClass& lhs, const RuntimeClass& rhs) noexcept { return cmp(lhs, rhs) >= 0; }
#endif
    };
}

#ifndef _MSC_VER
#ifndef SW_SHOW
#define SW_SHOW 5
#endif
#ifndef SW_HIDE
#define SW_HIDE 0
#endif
#ifndef SM_CXBORDER
#define SM_CXBORDER 5
#endif
#ifndef SM_CYBORDER
#define SM_CYBORDER 6
#endif

typedef struct tagRECT
{
    long left;
    long top;
    long right;
    long bottom;
} RECT;

class CRect : public tagRECT
{
};

typedef struct tagPOINT
{
    long x;
    long y;
} POINT;

class CPoint : public tagPOINT
{
public:
    CPoint(int initX, int initY) { x = initX; y = initY; }
};

typedef struct tagWINDOWPLACEMENT
{
    std::uint32_t length;
    std::uint32_t flags;
    std::uint32_t showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT rcNormalPosition;
} WINDOWPLACEMENT;

static int GetSystemMetrics(int val) { return 0; }

#endif


#ifndef _MSC_VER
#ifndef THREAD_PRIORITY_LOWEST
#define THREAD_PRIORITY_LOWEST (-2)
#endif

static void AfxThrowOleException(HRESULT hr) { throw std::logic_error("Failure"); }

#endif
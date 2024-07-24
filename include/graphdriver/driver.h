#if !defined(SORAGE_DRIVER_H)
#define SORAGE_DRIVER_H
// 定义ProtoDriver抽象基类
class ProtoDriver {
public:
    virtual ~ProtoDriver() {}
    virtual void Method1() = 0;
};

// 定义DiffDriver抽象基类
class DiffDriver {
public:
    virtual ~DiffDriver() {}
    virtual void Method2() = 0;
};

// 定义LayerIDMapUpdater抽象基类
class LayerIDMapUpdater {
public:
    virtual ~LayerIDMapUpdater() {}
    virtual void Method3() = 0;
};
class Driver:public ProtoDriver, public DiffDriver, public LayerIDMapUpdater {
public:
    virtual ~Driver() {}
};

#endif // SORAGE_DRIVER_H

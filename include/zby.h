#ifndef _ZBY_H
#define _ZBY_H
// NOTE: compile using gcc with '-fplan9-extensions' enabled

// https://github.com/zhangboyang/c-with-virtual-classes


//////////////////// useful macros ////////////////////////////////////////////

#define _TOSTR(x) #x
#define TOSTR(x) _TOSTR(x)
#define _CONCAT(a, b) a ## b
#define CONCAT(a, b) _CONCAT(a, b)
#define CONCAT3(a, b, c) CONCAT(CONCAT(a, b), c)
#define CONCAT4(a, b, c, d) CONCAT(CONCAT3(a, b, c), d)
#define CONCAT5(a, b, c, d, e) CONCAT(CONCAT4(a, b, c, d), e)
#define CONCAT6(a, b, c, d, e, f) CONCAT(CONCAT5(a, b, c, d, e), f)


//////////////////// lambda functions /////////////////////////////////////////
#define LAMBDA(ret_type, args, body...) \
    ({ \
        ret_type __c_with_lamda__temp_function args body \
        __c_with_lamda__temp_function; \
    })
#define L LAMBDA

//////////////////// class manipulate macros //////////////////////////////////

#define BASECLASS_CAST(ptr) (&(ptr)->baseclass)
#define BC BASECLASS_CAST
#define THIS void *vthis
#define DECLARE_THIS(type) struct type *this = vthis
#define MEMBER(var) (this->var)
#define M MEMBER


///////////// write derived class (without virtual funtcion) in pure C ////////

#define DECLARE_BASE_CLASS(base_class_name, base_class_structure...) \
    struct base_class_name { \
        base_class_structure; \
    }
#define DECLARE_DERIVED_CLASS(derived_class_name, base_class_name, derived_class_structure...) \
    struct derived_class_name { \
        union { \
            struct base_class_name; \
            struct base_class_name baseclass; \
        }; \
        derived_class_structure; \
    }



//////////////////// write virtual class in pure C ////////////////////////////

    // NOTE: you can't type comma without parenthesis in vfunc list
    //       for example, the following statment will cause an error
    //   void (*hello)(THIS), (*hello2)(THIS);
    //                      ^ no comma here!
    
#define DECLARE_BASE_VCLASS(base_class_name, base_class_vfunction, base_class_structure...) \
    struct base_class_name { \
        struct CONCAT(base_class_name, __vtbl) { \
            base_class_vfunction; \
        } *vfptr; \
        struct CONCAT(base_class_name, __data) { \
            base_class_structure; \
        }; \
    }

#define DECLARE_DERIVED_VCLASS(derived_class_name, base_class_name, derived_class_vfunction, derived_class_structure...) \
    struct derived_class_name { \
        union { \
            struct base_class_name baseclass; \
            struct { \
                struct CONCAT(derived_class_name, __vtbl) { \
                    struct CONCAT(base_class_name, __vtbl); \
                    derived_class_vfunction; \
                } *vfptr; \
                struct CONCAT(derived_class_name, __data) { \
                    struct CONCAT(base_class_name, __data); \
                    derived_class_structure; \
                }; \
            }; \
        }; \
    }

#define INST_VTBL_SINGLETON(ptr, vtbl_data...) \
    do { \
        static typeof(*((ptr)->vfptr)) __c_with_virtual_class__vtbl_singleton = vtbl_data; \
        (ptr)->vfptr = &__c_with_virtual_class__vtbl_singleton; \
    } while (0) \

#define VIRTUAL_FUNC(this, func, ...) ({ \
        typeof(this) __c_with_virtual_class__temp_this = (this); \
        __c_with_virtual_class__temp_this->vfptr->func(__c_with_virtual_class__temp_this, ##__VA_ARGS__); \
    })
#define VF VIRTUAL_FUNC


#endif


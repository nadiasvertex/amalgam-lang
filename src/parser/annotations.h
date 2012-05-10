/*
 * annotations.h
 *
 *  Created on: May 9, 2012
 *      Author: cnelson
 */

#ifndef ANNOTATIONS_H_
#define ANNOTATIONS_H_

#include <map>
#include <string>
#include <vector>

#include "types.h"

namespace amalgam {
namespace parser {

/** Enumerations of all different kinds of annotations. */
enum class annotation_type {
   method, type
};

/** Base type for all annotations. */
struct annotation {
   // The kind of annotation.
   annotation_type type;

   annotation(annotation_type _type) :
            type(_type) {
   }
   ;
};

/** An annotation which stores type information. */
struct type_annotation : public annotation {
   /** The type of pointers to type_annotations. */
   typedef std::shared_ptr<type_annotation> ptr_t;

   /** The type of lists of type annotations. */
   typedef std::vector<ptr_t> list_t;

   /** The type of lists of type annotations. */
   typedef std::map<string, ptr_t> map_t;

   /** The type of pointers to lists. */
   typedef std::unique_ptr<list_t> list_ptr_t;

   /** An enumeration of all the various classes
    * of types.
    */
   enum class type_id {
      integer, floating_point, string, astruct, tuple, dict, method, interface, generic
   };

   /** The name of the type. */
   string name;

   /** The class of type. */
   type_id id;

   /** True if this type is a constant.*/
   bool is_constant;

   /** True if this type is a vector. */
   bool is_vector;

   /** True if this type is an array. */
   bool is_array;

   /** The length of this type in elements, if is_vector or is_array is true. Scalar types have this set to 1, arrays have
    * this set to the length of the array, vectors have this set to the length of the vector.
    */
   uint64_t size_in_elements;

   type_annotation(type_id _id) :
            annotation(annotation_type::type), id(_id), is_constant(false), is_vector(false), is_array(false), size_in_elements(0) {
   }
   ;
};

struct numeric_type_annotation : public type_annotation {
   /** True if this type is a signed number. */
   bool is_signed;

   /** The length of this type in bits. */
   uint8_t size_in_bits;

   numeric_type_annotation(type_id _id) :
            is_signed(false), size_in_bits(0), type_annotation(_id) {
   }
   ;
};

struct composite_type_annotation : public type_annotation {
   /** If this is a composite type (struct or tuple), then children is non-null and contains the type annotations for
    * struct members. If this is a dict type, the first element in children is the type for the key, the second element
    * is the type for the value.
    */
   list_ptr_t children;

   composite_type_annotation(type_id _id) :
            type_annotation(_id) {
   }
   ;

};

struct method_type_annotation : public type_annotation {
   /** The list of outputs for a function. */
   list_ptr_t output;

   /** The list of inputs for a function. */
   list_ptr_t input;

   method_type_annotation() :
            type_annotation(type_id::method) {
   }
   ;
};

} // end parser namespace
} // end amalgam namespace

#endif /* ANNOTATIONS_H_ */

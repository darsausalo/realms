IF(MSVC)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
ENDIF()

MACRO(SETUP_CPP11 _targetName)
	IF(APPLE)
		SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
		SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
		SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD_64_BIT)")
		SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")
	ENDIF()
	IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++1z -stdlib=libc++")
	ENDIF()
ENDMACRO()

MACRO(SETUP_COMPILER)
	IF(APPLE)
		ADD_DEFINITIONS(-DITS_PLATFORM_APPLE=1)
		ADD_DEFINITIONS(-Wall -Wextra -pedantic)
        SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
    ELSEIF(UNIX)
        ADD_COMPILE_OPTIONS(-Wno-ignored-attributes)
	ELSEIF(WIN32)
		IF(MSVC)
			ADD_COMPILE_OPTIONS(-bigobj)
		ENDIF()
		ADD_DEFINITIONS(-DITS_PLATFORM_WIN32=1)
		ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
		string (REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
		ADD_DEFINITIONS("/W4")
		IF(MSVC)
			IF("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
				ADD_DEFINITIONS("-DDAS_ENABLE_FPE")
				ADD_DEFINITIONS("/arch:IA32")
				ADD_DEFINITIONS("-D_TARGET_SIMD_SSE=2")
			ENDIF()
		ENDIF()
		SET(CMAKE_CXX_FLAGS_RELEASE "/MD /Ox /Ob2 /Ot /Oi /DNDEBUG /Gy /GS- /GR- /EHa")
		SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Zi /MD /Ox /Ob2 /Ot /Oi /DNDEBUG /Gy /GS- /GR- /EHa")
	ENDIF()
ENDMACRO()

SETUP_COMPILER()

set(PARSER_GENERATED_SRC
	daScript/src/parser/ds_parser.hpp
	daScript/src/parser/ds_parser.cpp
	daScript/src/parser/ds_parser.output
	daScript/src/parser/ds_lexer.cpp
)

if(MSVC)
    # Don't use precompiled headers with flex/bison generated files
    foreach(file ${PARSER_GENERATED_SRC})
        set_source_files_properties(${file} PROPERTIES COMPILE_FLAGS "/Y-" )
    endforeach()
endif()

set(VECMATH_SRC
	daScript/include/vecmath/dag_vecMath.h
	daScript/include/vecmath/dag_vecMathDecl.h
	daScript/include/vecmath/dag_vecMath_common.h
	daScript/include/vecmath/dag_vecMath_const.h
	daScript/include/vecmath/dag_vecMath_neon.h
	daScript/include/vecmath/dag_vecMath_pc_sse.h
)

set(AST_SRC
	daScript/src/ast/ast.cpp
	daScript/src/ast/ast_generate.cpp
	daScript/src/ast/ast_simulate.cpp
	daScript/src/ast/ast_typedecl.cpp
	daScript/src/ast/ast_match.cpp
	daScript/src/ast/ast_module.cpp
	daScript/src/ast/ast_print.cpp
	daScript/src/ast/ast_aot_cpp.cpp
	daScript/src/ast/ast_plot.cpp
	daScript/src/ast/ast_infer_type.cpp
	daScript/src/ast/ast_lint.cpp
	daScript/src/ast/ast_allocate_stack.cpp
	daScript/src/ast/ast_const_folding.cpp
	daScript/src/ast/ast_block_folding.cpp
	daScript/src/ast/ast_unused.cpp
	daScript/src/ast/ast_annotations.cpp
	daScript/src/ast/ast_export.cpp
	daScript/src/ast/ast_parse.cpp
	daScript/src/ast/ast_debug_info_helper.cpp
	daScript/src/ast/ast_handle.cpp
	daScript/src/ast/ast_runtime_helpers.cpp
	daScript/include/daScript/ast/compilation_errors.h
	daScript/include/daScript/ast/ast_typedecl.h
	daScript/include/daScript/ast/ast_typefactory.h
	daScript/include/daScript/ast/ast.h
	daScript/include/daScript/ast/ast_expressions.h
	daScript/include/daScript/ast/ast_visitor.h
	daScript/include/daScript/ast/ast_generate.h
	daScript/include/daScript/ast/ast_match.h
	daScript/include/daScript/ast/ast_interop.h
	daScript/include/daScript/ast/ast_handle.h
	daScript/include/daScript/ast/ast_policy_types.h
)

set(BUILTIN_SRC
	daScript/src/builtin/module_builtin.h
	daScript/src/builtin/module_builtin.cpp
	daScript/src/builtin/module_builtin_misc_types.cpp
	daScript/src/builtin/module_builtin_runtime.cpp
	daScript/src/builtin/module_builtin_runtime_sort.cpp
	daScript/src/builtin/module_builtin_vector.cpp
	daScript/src/builtin/module_builtin_vector_ctor.cpp
	daScript/src/builtin/module_builtin_array.cpp
	daScript/src/builtin/module_builtin_das.cpp
	daScript/src/builtin/module_builtin_math.cpp
	daScript/src/builtin/module_builtin_random.cpp
	daScript/src/builtin/module_builtin_string.cpp
	daScript/src/builtin/module_builtin_rtti.h
	daScript/src/builtin/module_builtin_rtti.cpp
	daScript/src/builtin/module_builtin_ast.cpp
	daScript/src/builtin/module_builtin_fio.cpp
	daScript/src/builtin/module_builtin_network.cpp
	daScript/src/builtin/module_file_access.cpp
	daScript/src/builtin/builtin.das.inc
	daScript/src/builtin/builtin.das
	daScript/src/builtin/fio.das.inc
	daScript/src/builtin/fio.das
	daScript/src/builtin/rtti.das.inc
	daScript/src/builtin/rtti.das
	daScript/src/builtin/ast.das.inc
	daScript/src/builtin/ast.das
	daScript/src/builtin/network.das.inc
	daScript/src/builtin/network.das
)

set(MISC_SRC
	daScript/include/daScript/misc/enums.h
	daScript/include/daScript/misc/function_traits.h
	daScript/include/daScript/misc/hal.h
	daScript/include/daScript/misc/fpe.h
	daScript/include/daScript/misc/copy_bytes.h
	daScript/include/daScript/misc/platform.h
	daScript/include/daScript/misc/vectypes.h
	daScript/include/daScript/misc/arraytype.h
	daScript/include/daScript/misc/rangetype.h
	daScript/include/daScript/misc/string_writer.h
	daScript/include/daScript/misc/type_name.h
	daScript/include/daScript/misc/lookup1.h
	daScript/include/daScript/misc/memory_model.h
	daScript/include/daScript/misc/fnv.h
	daScript/include/daScript/misc/smart_ptr.h
	daScript/include/daScript/misc/free_list.h
	daScript/include/daScript/misc/sysos.h
	daScript/src/misc//sysos.cpp
	daScript/src/misc/string_writer.cpp
	daScript/src/misc/memory_model.cpp
)

set(SIMULATE_FUSION_SRC
	daScript/src/simulate/simulate_fusion.cpp
	daScript/src/simulate/simulate_fusion_op1.cpp
	daScript/src/simulate/simulate_fusion_op1_return.cpp
	daScript/src/simulate/simulate_fusion_ptrfdr.cpp
	daScript/src/simulate/simulate_fusion_op2.cpp
	daScript/src/simulate/simulate_fusion_op2_set.cpp
	daScript/src/simulate/simulate_fusion_op2_bool.cpp
	daScript/src/simulate/simulate_fusion_op2_bin.cpp
	daScript/src/simulate/simulate_fusion_op2_vec.cpp
	daScript/src/simulate/simulate_fusion_op2_set_vec.cpp
	daScript/src/simulate/simulate_fusion_op2_bool_vec.cpp
	daScript/src/simulate/simulate_fusion_op2_bin_vec.cpp
	daScript/src/simulate/simulate_fusion_op2_scalar_vec.cpp
	daScript/src/simulate/simulate_fusion_at.cpp
	daScript/src/simulate/simulate_fusion_at_array.cpp
	daScript/src/simulate/simulate_fusion_tableindex.cpp
	daScript/src/simulate/simulate_fusion_misc_copy.cpp
	daScript/src/simulate/simulate_fusion_call1.cpp
	daScript/src/simulate/simulate_fusion_call2.cpp
	daScript/src/simulate/simulate_fusion_if.cpp
	daScript/include/daScript/simulate/simulate_fusion.h
	daScript/include/daScript/simulate/simulate_fusion_op1.h
	daScript/include/daScript/simulate/simulate_fusion_op1_impl.h
	daScript/include/daScript/simulate/simulate_fusion_op1_perm.h
	daScript/include/daScript/simulate/simulate_fusion_op1_set_impl.h
	daScript/include/daScript/simulate/simulate_fusion_op1_set_perm.h
	daScript/include/daScript/simulate/simulate_fusion_op1_reg.h
	daScript/include/daScript/simulate/simulate_fusion_op2.h
	daScript/include/daScript/simulate/simulate_fusion_op2_impl.h
	daScript/include/daScript/simulate/simulate_fusion_op2_perm.h
	daScript/include/daScript/simulate/simulate_fusion_op2_set_impl.h
	daScript/include/daScript/simulate/simulate_fusion_op2_set_perm.h
	daScript/include/daScript/simulate/simulate_fusion_op2_vec_settings.h
)

set(SIMULATE_SRC
	daScript/src/hal/performance_time.cpp
	daScript/include/daScript/misc/performance_time.h
	daScript/src/hal/debug_break.cpp
	daScript/src/hal/project_specific.cpp
	daScript/src/hal/project_specific_file_info.cpp
	daScript/include/daScript/misc/network.h
	daScript/src/misc/network.cpp
	daScript/src/simulate/hash.cpp
	daScript/src/simulate/debug_info.cpp
	daScript/src/simulate/runtime_string.cpp
	daScript/src/simulate/runtime_array.cpp
	daScript/src/simulate/runtime_table.cpp
	daScript/src/simulate/runtime_range.cpp
	daScript/src/simulate/runtime_profile.cpp
	daScript/src/simulate/simulate.cpp
	daScript/src/simulate/simulate_gc.cpp
	daScript/src/simulate/simulate_visit.cpp
	daScript/src/simulate/simulate_print.cpp
	daScript/src/simulate/simulate_fn_hash.cpp
	daScript/include/daScript/simulate/cast.h
	daScript/include/daScript/simulate/hash.h
	daScript/include/daScript/simulate/heap.h
	daScript/src/simulate/heap.cpp
	daScript/include/daScript/simulate/debug_info.h
	daScript/include/daScript/simulate/interop.h
	daScript/include/daScript/simulate/runtime_string.h
	daScript/include/daScript/simulate/runtime_string_delete.h
	daScript/include/daScript/simulate/runtime_array.h
	daScript/include/daScript/simulate/runtime_table.h
	daScript/include/daScript/simulate/runtime_table_nodes.h
	daScript/include/daScript/simulate/runtime_range.h
	daScript/include/daScript/simulate/runtime_profile.h
	daScript/include/daScript/simulate/runtime_matrices.h
	daScript/include/daScript/simulate/simulate.h
	daScript/include/daScript/simulate/simulate_nodes.h
	daScript/include/daScript/simulate/simulate_visit.h
	daScript/include/daScript/simulate/simulate_visit_op.h
	daScript/include/daScript/simulate/simulate_visit_op_undef.h
	daScript/include/daScript/simulate/sim_policy.h
	daScript/src/simulate/data_walker.cpp
	daScript/include/daScript/simulate/data_walker.h
	daScript/src/simulate/debug_print.cpp
	daScript/include/daScript/simulate/debug_print.h
	daScript/include/daScript/simulate/for_each.h
	daScript/include/daScript/simulate/bind_enum.h
	daScript/include/daScript/simulate/bin_serializer.h
	daScript/src/simulate/bin_serializer.cpp
	daScript/include/daScript/simulate/aot.h
	daScript/include/daScript/simulate/aot_library.h
	daScript/include/daScript/simulate/aot_builtin.h
	daScript/include/daScript/simulate/aot_builtin_math.h
	daScript/include/daScript/simulate/aot_builtin_matrix.h
	daScript/include/daScript/simulate/aot_builtin_random.h
	daScript/include/daScript/simulate/aot_builtin_time.h
	daScript/include/daScript/simulate/aot_builtin_string.h
	daScript/include/daScript/simulate/aot_builtin_fio.h
	daScript/include/daScript/simulate/aot_builtin_rtti.h
	daScript/include/daScript/simulate/aot_builtin_ast.h
	daScript/include/daScript/simulate/fs_file_info.h
	daScript/src/simulate/fs_file_info.cpp
)

set(DAGOR_NOISE_SRC
	daScript/include/dag_noise/dag_uint_noise.h
)

set(FLAT_HASH_MAP_SRC
	daScript/include/ska/flat_hash_map.hpp
)

set(MAIN_SRC
	daScript/include/daScript/daScript.h
	daScript/include/daScript/daScriptModule.h
	daScript/include/daScript/das_config.h
)

file(GLOB DAS_LIB_SRC
	"daScript/daslib/*.das"
)

include_directories("daScript/include")

add_library(libDaScript ${VECMATH_SRC} ${AST_SRC} ${BUILTIN_SRC} ${MISC_SRC} ${SIMULATE_SRC}
    ${SIMULATE_FUSION_SRC} ${TEST_SRC} ${MAIN_SRC} ${PARSER_SRC} ${PARSER_GENERATED_SRC}
	${DAGOR_NOISE_SRC} ${FLAT_HASH_MAP_SRC} ${DAS_LIB_SRC})
SETUP_CPP11(libDaScript)


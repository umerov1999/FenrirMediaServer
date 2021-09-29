// XTPEnableAdvancedWarnings.h: Enables/Disables advanced MSVC compiler
// warnings that are either turned off by default or enabled by /Wall switch.
//
// (c)1998-2021 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE

// Advanced warning control is required only for compilation with /Wall switch.
#if defined(_MSC_VER) && (1900 <= _MSC_VER)

#	ifndef XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS_H_INCLUSIONS

#		define XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS_H_INCLUSIONS

#		ifdef XTP_INTERNAL_WARNING_CONTROL
// clang-format off
// Controlling only noisy /Wall warnings, those that don't indicate actual bugs:
// C4615: #pragma warning : unknown user warning type
// C4616: #pragma warning : warning number 'number' out of range, must be between 'number' and 'number'
// C4710: 'function': function not inlined
// C4275: non dll - interface class 'user-defined type' used as base for dll - interface class 'user-defined type'
// C4820: 'user-defined type' : '3' bytes padding added after data member 'user-defined type'
// C4625: 'user-defined type' : copy constructor was implicitly defined as deleted
// C4626: 'user-defined type' : assignment operator was implicitly defined as deleted
// C5026: 'user-defined type' : move constructor was implicitly defined as deleted
// C5027: 'user-defined type' : move assignment operator was implicitly defined as deleted
// C4251: 'identifier' : class 'user-defined type' needs to have dll - interface to be
// used by clients of class 'user-defined type'
// C4555: expression has no effect; expected expression with side-effect
// C4623: 'user-defined type': default constructor was implicitly defined as deleted
// C4514: 'function' : unreferenced inline function has been removed
// C4571: Informational: catch (...) semantics changed since Visual C++ 7.1; structured exceptions(SEH) are no longer caught
// C4371: layout of class may have changed from a previous version of the compiler due to
// better packing of member 'member'
// C5039: 'function' : pointer or reference to potentially throwing function passed to extern C 
// function under - EHc. Undefined behavior may occur if this function throws an exception.
// C4768: __declspec attributes before linkage specification are ignored
// C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
// C4061: enumerator '...' in switch of enum '...' is not explicitly handled by a case label
// C4062: enumerator '...' in switch of enum '...' is not handled
// C4548: expression before comma has no effect; expected expression with side-effect
// C4986: '...': exception specification does not match previous declaration
// C2143: syntax error: missing ';' before '...'
// C4430: missing type specifier - int assumed. Note: C++ does not support default-int
// C4183: '...': missing return type; assumed to be a member function returning 'int'
// C4097: typedef-name 'Parent' used as synonym for class-name '...'
// C4711: function '...' selected for automatic inline expansion
// clang-format on
#			define XTP_ADVANCED_WARNING_CONTROL(action)                                           \
				__pragma(warning(                                                                  \
					action : 4097 4615 4616 4710 4514 4275 4820 4625 4626 5026 5027 4251 4555 4623 4571 4371 5039 4768 5045 4062 4061 4548 4986 2143 4430 4183 4711))
#		else // XTP_INTERNAL_WARNING_CONTROL
// clang-format off
// Controlling /Wall warnings:
// C4061: (level 4) enumerator 'identifier' in a switch of enum 'enumeration' is not explicitly
// handled by a case label
// C4062: (level 3) enumerator 'identifier' in a switch of enum 'enumeration' is not handled
// C4191: (level 3) 'operator/operation': unsafe conversion from 'type of expression' to 'type required'
// C4242: (level 4) 'identifier': conversion from 'type1' to 'type2', possible loss of data
// C4254: (level 4) 'operator': conversion from 'type1' to 'type2', possible loss of data
// C4255: (level 4) 'function': no function prototype given: converting '()' to '(void)'
// C4263: (level 4) 'function': member function does not override any base class virtual member function
// C4264: (level 1) 'virtual_function': no override available for virtual member function from base 'class'; function is hidden
// C4265: (level 3) 'class': class has virtual functions, but destructor is not virtual
// C4266: (level 4) 'function': no override available for virtual member function from base 'type'; function is hidden
// C4287: (level 3) 'operator': unsigned/negative constant mismatch
// C4289: (level 4) nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope
// C4296: (level 4) 'operator': expression is always false
// C4302: (level 2) 'conversion': truncation from 'type1' to 'type2'
// C4350: (level 1) behavior change: 'member1' called instead of 'member2'
// C4365: (level 4) 'action': conversion from 'type_1' to 'type_2', signed/unsigned mismatch
// C4412: (level 2) 'function': function signature contains type 'type'; 
// C++ objects are unsafe to pass between pure code and mixed or native
// C4431: (level 4) missing type specifier - int assumed. Note: C no longer supports default-int
// C4514: (level 4) 'function': unreferenced inline function has been removed
// C4536: (level 4) 'type name': type-name exceeds meta-data limit of 'limit' characters
// C4545: (level 1) expression before comma evaluates to a function which is missing an argument list
// C4546: (level 1) function call before comma missing argument list C4547: (level 1)
// 'operator': operator before comma has no effect; expected operator with side-effect
// C4548: (level 1) expression before comma has no effect; expected expression with side-effect
// C4549: (level 1) 'operator': operator before comma has no effect; did you intend 'operator'?
// C4555: (level 1) expression has no effect; expected expression with side-effect
// C4557: (level 3) '__assume' contains effect 'effect'
// C4571: (level 4) informational: catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
// C4619: (level 3) #pragma warning: there is no warning number 'number'
// C4623: (level 4) 'derived class': default constructor could not be generated because a base class default constructor is inaccessible
// C4625: (level 4) 'derived class': copy constructor could not be generated because a base class
// copy constructor is inaccessible
// C4626: (level 4) 'derived class': assignment operator could not be generated because a base class assignment operator is inaccessible
// C4628: (level 1) digraphs not supported with -Ze. Character sequence 'digraph' not interpreted as alternate token for 'char'
// C4640: (level 3) 'instance': construction of local static object is not thread-safe
// C4641: (level 3) XML document comment has an ambiguous cross reference
// C4668: (level 4) 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
// C4686: (level 3) 'user-defined type': possible change in behavior, change in UDT return calling convention
// C4692: (level 1) 'function': signature of non-private member contains assembly private native type 'native_type'
// C4710: (level 4) 'function': function not inlined
// C4711: (level 1) function 'function' selected for inline expansion
// C4738: (level 3) storing 32-bit float result in memory, possible loss of performance
// C4820: (level 4) 'bytes' bytes padding added after construct 'member_name'
// C4826: (level 2) conversion from 'type1' to 'type2' is sign-extended. This may cause unexpected runtime behavior
// C4836: (level 1) nonstandard extension used : 'type' : local types or unnamed types cannot be used as template arguments
// C4837: (level 4) trigraph detected: '??%c' replaced by '%c'
// C4905: (level 1) wide string literal cast to 'LPSTR'
// C4906: (level 1) string literal cast to 'LPWSTR'
// C4917: (level 1) 'declarator': a GUID can only be associated with a class, interface, or namespace
// C4928: (level 1) illegal copy-initialization; more than one user-defined conversion has been implicitly applied
// C4931: (level 4) we are assuming the type library was built for number-bit pointers
// C4946: (level 1) reinterpret_cast used between related classes: 'class1' and 'class2'
// C4962: 'function': profile-guided optimizations disabled because optimizations caused profile data to become inconsistent
// C4987: nonstandard extension used: 'throw (...)'
// C5026: 'user-defined type': move constructor was implicitly defined as deleted
// C5027: 'user-defined type': move assignment operator was implicitly defined as deleted
// C4471: 'user-defined type': a forward declaration of an unscoped enumeration must have an underlying type(int assumed)
// C4201: nonstandard extension used : nameless struct / union
// C4458: declaration of 'identifier' hides class member
// C5039: 'function' : pointer or reference to potentially throwing function passed to
// extern C function under - EHc. Undefined behavior may occur if this function throws an exception.
// C4768: __declspec attributes before linkage specification are ignored
// C4986: 'function': exception specification does not match previous declaration
// C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
// C5204: '...': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
// C5220: '...': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial
// C5038: data member '...' will be initialized after data member '...'
// C4131: '...': uses old-style declarator
// C4371: layout of class may have changed from a previous version of the compiler due to
// C4097: typedef-name 'Parent' used as synonym for class-name '...'
// C5246: '...': the initialization of a subobject should be wrapped in braces
// clang-format on
#			define XTP_ADVANCED_WARNING_CONTROL(action)                                           \
				__pragma(warning(                                                                  \
					action : 4097 4061 4062 4191 4201 4242 4254 4255 4263 4264 4265 4266 4287 4289 4296 4302 4350 4365 4412 4431 4514 4536 4545 4546 4547 4548 4549 4555 4557 4571 4619 4623 4625 4626 4628 4640 4641 4668 4686 4692 4710 4711 4738 4820 4826 4836 4837 4905 4906 4917 4928 4931 4946 4962 4987 5026 5027 4710 4471 4458 5039 4768 4986 5045 5204 5220 5038 4131 4371 5246))
#		endif /*!XTP_INTERNAL_WARNING_CONTROL*/

#		ifdef XTP_DISABLE_ADVANCED_WARNINGS
#			define XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS__DISABLE
#			define XTP_WARNING_CONTROL_ACTION disable
#			undef XTP_DISABLE_ADVANCED_WARNINGS
#		else // XTP_DISABLE_ADVANCED_WARNINGS
#			define XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS__ENABLE
#			define XTP_WARNING_CONTROL_ACTION default
#		endif // !XTP_DISABLE_ADVANCED_WARNINGS

#		ifdef XTP_INTERNAL_WARNING_CONTROL
// C5032: detected #pragma warning(push) with no corresponding #pragma warning(pop)
#			pragma warning(disable : 5032)

// Prevent C4435: class1' : Object layout under /vd2 will change due to virtual base 'class2'
#			pragma vtordisp(push, off)

#			undef XTP_INTERNAL_WARNING_CONTROL
#		endif

#		pragma warning(push)

// C4615: #pragma warning:	unknown user warning type
#		pragma warning(disable : 4615)

XTP_ADVANCED_WARNING_CONTROL(XTP_WARNING_CONTROL_ACTION)

// C4615: #pragma warning:	unknown user warning type
#		pragma warning(default : 4615)

#		undef XTP_ADVANCED_WARNING_CONTROL

#	else // XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS_H_INCLUSIONS

#		undef XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS_H_INCLUSIONS

#		ifdef XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS__DISABLE
#			ifdef XTP_DISABLE_ADVANCED_WARNINGS
#error XTP_DISABLE_ADVANCED_WARNINGS must not be defined twice. The state of advanced warnings must be restored.
#			endif
#			undef XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS__DISABLE
#		endif

#		ifdef XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS__ENABLE
#			ifndef XTP_DISABLE_ADVANCED_WARNINGS
#error XTP_DISABLE_ADVANCED_WARNINGS must be defined in order to restore the state of advanced warnings.
#			endif
#			undef XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS__ENABLE
#		endif

#		ifdef XTP_INTERNAL_WARNING_CONTROL
#			pragma vtordisp(pop)
#			undef XTP_INTERNAL_WARNING_CONTROL
#		endif

#		pragma warning(pop)

#	endif // !XTP_BETWEEN_XTPENABLEADVANCEDWARNINGS_H_INCLUSIONS

#else // defined(_MSC_VER) && (1900  <= _MSC_VER)

#	ifdef XTP_INTERNAL_WARNING_CONTROL
#		undef XTP_INTERNAL_WARNING_CONTROL
#	endif

#	ifdef XTP_DISABLE_ADVANCED_WARNINGS
#		undef XTP_DISABLE_ADVANCED_WARNINGS
#	endif

#endif // !(defined(_MSC_VER) && (1900  <= _MSC_VER))

//}}AFX_CODEJOCK_PRIVATE

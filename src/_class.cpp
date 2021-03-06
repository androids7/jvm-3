#include <iostream>
#include <iomanip>
#include <string>
#include <bit.hpp>
#include "bit.hpp"
#include "macros.hpp"
#include "_class.hpp"

namespace jvm {

	_Class::_Class() = default;

	void _Class::read_attributes (jvm::Reader &file) {
		attributes_count = file.getNextHalfWord();

		if (attributes_count == 0) {
			return;
		}

		for (int i = 0; i < attributes_count; ++i) {
			attributes.emplace_back(AttributeInfo(file));
		}
	}

	void _Class::read_methods (jvm::Reader &file) {
		methods_count = file.getNextHalfWord();

		if (methods_count == 0) {
			return;
		}

		for (int i = 0; i < methods_count; ++i) {
			methods.emplace_back(MethodInfo(file));
		}
	}

	void _Class::read_fields (jvm::Reader &file) {
		fields_count = file.getNextHalfWord();

		if (fields_count == 0) {
			return;
		}

		for (int i = 0; i < fields_count; ++i) {
			fields.emplace_back(FieldInfo(file));
		}
	}

	void _Class::read_interfaces (jvm::Reader &file) {
		interfaces_count = file.getNextHalfWord();

		if (interfaces_count == 0) {
			return;
		}

		for (int i = 0; i < interfaces_count; ++i) {
			interfaces.push_back(InterfaceInfo(file));
		}
	}

	void _Class::read_flags (jvm::Reader &file) {
		access_flags = file.getNextHalfWord();
		this_class = file.getNextHalfWord();
		super_class = file.getNextHalfWord();
	}

	void _Class::read_cp (jvm::Reader& file) {
		cp_count = file.getNextHalfWord() - 1;

		if (cp_count <= 0) {
			return;
		}

		constant_pool.fill(file, cp_count);
	}

	void _Class::read_version (jvm::Reader& file) {
		magic_number = MAGIC_NUMBER;
		min_version = file.getNextHalfWord();
		max_version = file.getNextHalfWord();
	}

	/**
	 * Read the constant pool data
	 * @param file The file to extract the data
	 * @param count How many constants to extract
	 */
	void _Class::read (std::basic_string<char> filename) {
		auto file = jvm::Reader();

		file.open(filename);

		read_version(file);
		read_cp(file);
		read_flags(file);
		read_interfaces(file);
		read_fields(file);
		read_methods(file);
		read_attributes(file);

		file.close();
	}

	void _Class::print_class_flags() {
		auto flag = (uint32_t) access_flags;
		std::cout << "Access Flags:" << std::endl;

		if (flag == 0) {
			std::cout << "\t -o-" << std::endl;
			return;
		}

		{
			using namespace jvm::_class;

			if (flag & Flags::PUBLIC)     std::cout << "\tPublic"     << std::endl;
			if (flag & Flags::PRIVATE)    std::cout << "\tPrivate"    << std::endl;
			if (flag & Flags::PROTECTED)  std::cout << "\tProtected"  << std::endl;
			if (flag & Flags::STATIC)     std::cout << "\tStatic"     << std::endl;
			if (flag & Flags::FINAL)      std::cout << "\tFinal"      << std::endl;
			if (flag & Flags::SUPER)      std::cout << "\tSuper"      << std::endl;
			if (flag & Flags::VOLATILE)   std::cout << "\tVolatile"   << std::endl;
			if (flag & Flags::TRANSIENT)  std::cout << "\tTransient"  << std::endl;
			if (flag & Flags::INTERFACE)  std::cout << "\tInterface"  << std::endl;
			if (flag & Flags::ABSTRACT)   std::cout << "\tAbstract"   << std::endl;
			if (flag & Flags::SYNTHETIC)  std::cout << "\tSynthetic"  << std::endl;
			if (flag & Flags::ANNOTATION) std::cout << "\tAnnotation" << std::endl;
			if (flag & Flags::ENUM)       std::cout << "\tEnum"       << std::endl;
		}
	}

	void _Class::print_attributes () {
		std::cout << "Attributes Count: " << attributes_count << std::endl;

		if (attributes_count == 0) {
			return;
		}

		std::cout << "Attributes:";

		auto i = 0;
		for (auto& attribute : attributes) {
			std::cout << std::endl << "\t[" << std::setfill('0') << std::setw(2) << ++i << "] ";
			attribute.printToStream(std::cout, constant_pool, "\t");
		}
	}

	void _Class::print_methods () {
		std::cout << "Methods Count: " << methods_count << std::endl;

		if (methods_count == 0) {
			return;
		}

		std::cout << "Methods:";

		auto i = 0;
		for (auto& method : methods) {
			std::cout << std::endl << "\t[" << std::setfill('0') << std::setw(2) << ++i << "] ";
			method.PrintToStream(std::cout, constant_pool);
		}
	}

	void _Class::print_fields () {
		std::cout << "Fields Count: " << fields_count << std::endl;

		if (fields_count == 0) {
			return;
		}

		std::cout << "Fields:";

		auto i = 0;
		for (auto& field : fields) {
			std::cout << std::endl << "\t[" << std::setfill('0') << std::setw(2) << ++i << "] ";
			field.PrintToStream(std::cout, constant_pool);
		}
	}

	void _Class::print_interfaces () {
		std::cout << "Interfaces Count: " << interfaces_count << std::endl;

		if (interfaces_count == 0) {
			return;
		}

		std::cout << "Interfaces:";

		auto i = 0;
		for (auto& interface : interfaces) {
			std::cout << std::endl << "\t[" << std::setfill('0') << std::setw(2) << ++i << "] ";
			interface.PrintToStream(std::cout, constant_pool);
		}
	}

	void _Class::print_this_class () {
		CP_Entry* value = constant_pool[this_class];
		std::cout << "Classes:"<< std::endl;
		std::cout << "\t";
		value->printToStream(std::cout, constant_pool);
	}

	void _Class::print_super_class () {
		if (super_class == 0) {
			std::cout << "Object" << std::endl;
		} else {
			CP_Entry* value = constant_pool[super_class];
			std::cout << "Super Classes:"<< std::endl;
			std::cout << "\t";
			value->printToStream(std::cout, constant_pool);
		}
	}

	void _Class::print_cp () {
		std::cout << "Constant Pool Count: " << cp_count << std::endl;

		if (cp_count == 0) {
			return;
		}

		std::cout << "Constant Pool:";

		constant_pool.printToStream(std::cout);
	}

	void _Class::print_version () {
		std::cout << "Min Version: " << min_version << std::endl;
		std::cout << "Max Version: " << max_version << std::endl;
	}

	void _Class::show () {
		std::cout << "> .class" << std::endl << std::endl;

		print_version();
		print_cp();
		print_class_flags();
		print_this_class();
		print_super_class();
		print_interfaces();
		print_fields();
		print_methods();
		print_attributes();
	}

}
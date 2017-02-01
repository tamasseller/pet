/*******************************************************************************
 *
 * Copyright (c) 2016 Seller Tamás. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#ifndef ERROR_H_
#define ERROR_H_

#include "Failable.h"

namespace ubiq {

/**
 * Pointer that is null on error.
 *
 * This class can be used by a method returning a pointer, that can encounter exceptional
 * cases in which it returns a 0 pointer value.
 *
 * @tparam T The type a pointer to which is wrapped.
 */
template<class T>
class FailPointer: public Failable<FailPointer<T>, T*> {
	friend Failable<FailPointer<T>, T*>;
	inline bool isError() {
		return this->value == 0;
	}
public:
	/** @copydoc ubiq::FailableBase::FailableBase() */
	inline FailPointer(T* value): Failable<FailPointer<T>, T*>(value) {}

	/**
	 * Default copy constructor.
	 *
	 * Copies over the checkedness.
	 */
	inline FailPointer(const FailPointer &old) = default;

	/**
	 * Cast to anything.
	 *
	 * This operator is used for casting to anything through T*.
	 */
	template<class U>
	inline operator U() {
		return (U)(T*)(*this);
	};
};

/**
 * Some type that has one error value.
 *
 * This class can be used by a method returning some simple type, that can encounter
 * exceptional cases in which it returns a single predefined error value (for example -1).
 *
 * @tparam T The type to be wrapped.
 * @tparam errorValue The value that represents the error.
 */
template<class T, T errorValue>
class FailValue: public Failable<FailValue<T, errorValue>, T> {
	friend Failable<FailValue<T, errorValue>, T>;
	inline bool isError() {
		return this->value == errorValue;
	}
public:
	/** @copydoc ubiq::FailableBase::FailableBase() */
	inline FailValue(T value): Failable<FailValue<T, errorValue>, T>(value) {}

	/**
	 * Default copy constructor.
	 *
	 * Copies over the checkedness.
	 */
	inline FailValue(const FailValue &old) = default;

	/**
	 * Cast to anything.
	 *
	 * This operator is used for casting to anything through T.
	 */
	template<class U>
	inline operator U() {
		return (U)(T)(*this);
	};

	/** @copydoc ubiq::FailableBase::rethrow() */
	FailValue rethrow() {
		return FailValue(this->value);
	}
};

/**
 * Errno type error.
 *
 * This class can be used by a method normally returning a positive integer,
 * that can encounter exceptional cases in which it returns one of the
 * predefined error values. The error values are the same as the same as in
 * the errno.h header of stdlibc.
 */
class GenericError: public Failable<GenericError, int> {
	friend Failable<GenericError, int>;
	inline bool isError() {
		return this->value < 0;
	}

public:
	/**
	 * EPERM.
	 *
	 * Operation not permitted.
	 */
	static constexpr int insufficientPermission = -1;

	/**
	 * ENOENT.
	 *
	 * No such file or directory.
	 */
	static constexpr int noSuchEntry = -2;

	/**
	 * EIO.
	 *
	 * I/O error.
	 */
	static constexpr int cannotRead = -5;

	/**
	 * ENOMEM.
	 *
	 * Out of memory.
	 */
	static constexpr int outOfMemory = -12;

	/**
	 * EBUSY.
	 *
	 * Device or resource busy.
	 */
	static constexpr int resourceBusy = -16;

	/**
	 * EEXIST.
	 *
	 * File exists.
	 */
	static constexpr int alreadyExists = -17;

	/**
	 * ENOTDIR.
	 *
	 * Not a directory.
	 */
	static constexpr int isNotDirectory = -20;

	/**
	 * EISDIR.
	 *
	 * Is a directory.
	 */
	static constexpr int isDirectory = -21;

	/**
	 * EINVAL.
	 *
	 * Invalid argument.
	 */
	static constexpr int invalidArgument = -22;

	/**
	 * ENFILE.
	 *
	 * File table overflow.
	 */
	static constexpr int outOfFileStructures = -23;

	/**
	 * EFBIG.
	 *
	 * File too large.
	 */
	static constexpr int fileTooBig = -27;

	/**
	 * ENOSPC.
	 *
	 * No space left on device.
	 */
	static constexpr int cannotWrite = -28;

	/**
	 * ESPIPE.
	 *
	 * Illegal seek.
	 */
	static constexpr int invalidSeek = -29;

	/**
	 * EROFS.
	 *
	 * Read-only file system.
	 */
	static constexpr int readOnlyFs = -30;

	/**
	 * ENOTEMPTY.
	 *
	 * Directory not empty.
	 */
	static constexpr int notEmpty = -39;

	/** @cond */

	static GenericError insufficientPermissionError() {
		return GenericError(insufficientPermission);
	}


	static GenericError noSuchEntryError() {
		return GenericError(noSuchEntry);
	}

	static GenericError readError() {
		return GenericError(cannotRead);
	}

	static GenericError outOfMemoryError() {
		return GenericError(outOfMemory);
	}

	static GenericError alreadyInUseError() {
		return GenericError(resourceBusy);
	}

	static GenericError alreadyExistsError() {
		return GenericError(alreadyExists);
	}

	static GenericError isNotDirectoryError() {
		return GenericError(isNotDirectory);
	}

	static GenericError isDirectoryError() {
		return GenericError(isDirectory);
	}

	static GenericError invalidArgumentError() {
		return GenericError(invalidArgument);
	}

	static GenericError outOfFileStructuresError() {
		return GenericError(outOfFileStructures);
	}

	static GenericError fileTooBigError() {
		return GenericError(fileTooBig);
	}

	static GenericError writeError() {
		return GenericError(cannotWrite);
	}

	static GenericError invalidSeekError() {
		return GenericError(invalidSeek);
	}

	static GenericError readOnlyFsError() {
		return GenericError(readOnlyFs);
	}

	static GenericError notEmptyError() {
		return GenericError(notEmpty);
	}

	/** @endcond */

	/**
	 * Get error if any.
	 *
	 * Returns the error code if the value represents an error
	 * condition (negative value), 0 otherwise.
	 */
	inline int getErrorCode() {
		return (this->value < 0) ? this->value : 0;
	}

	/** @copydoc FailValue::FailValue(const FailValue&) */
	inline GenericError(const GenericError &old): Failable<GenericError, int>(old) {}

	/** @copydoc ubiq::FailableBase::FailableBase() */
	inline GenericError(int value): Failable<GenericError, int>(value) {}

	/**
	 * Check if not zero.
	 *
	 * Useful for shorthand conditions in if/while/for statements.
	 */
	inline operator bool() const {
		return (int)(*this) != 0;
	};

	inline const char* description() {
		static constexpr int insufficientPermission = -1;

		switch(value) {
			default:
				return "No error";

			case noSuchEntry:
				return "No such file or directory";

			case cannotRead:
				return "I/O error";

			case outOfMemory:
				return "Out of memory";

			case resourceBusy:
				return "Device or resource busy";

			case alreadyExists:
				return "File exists";

			case isNotDirectory:
				return "Not a directory";

			case isDirectory:
				return "Is a directory";

			case invalidArgument:
				return "Invalid argument";

			case outOfFileStructures:
				return "File table overflow";

			case fileTooBig:
				return "File too large";

			case cannotWrite:
				return "No space left on device";

			case invalidSeek:
				return "Illegal seek";

			case readOnlyFs:
				return "Read-only file system";

			case notEmpty:
				return "Directory not empty";
		}
	}
};

}

#endif /* ERROR_H_ */

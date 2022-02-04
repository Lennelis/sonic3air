/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2022 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "lemon/pch.h"
#include "lemon/utility/FlyweightString.h"


namespace lemon
{
	void FlyweightString::set(uint64 hash, std::string_view name)
	{
		using Entry = detail::FlyweightStringManager::Entry;
		Entry*& entry = mManager.mEntryMap[hash];
		if (nullptr == entry)
		{
			// Allocate enough memory to hold both the Entry struct and the string content
			const size_t requiredSize = sizeof(Entry) + name.length();
			uint8* entryPointer = mManager.mAllocPool.allocateMemory(requiredSize);

			// Initialize the entry and data
			mEntry = new (static_cast<void*>(entryPointer)) Entry();
			uint8* contentPointer = entryPointer + sizeof(Entry);
			memcpy(contentPointer, name.data(), name.length());

			mEntry->mHash = hash;
			mEntry->mString = std::string_view((const char*)contentPointer, name.length());
			entry = mEntry;
		}
		else
		{
			mEntry = entry;
		}
	}

	void FlyweightString::set(std::string_view name)
	{
		set(rmx::getMurmur2_64(name), name);
	}

	void FlyweightString::serialize(VectorBinarySerializer& serializer)
	{
		if (serializer.isReading())
		{
			const uint64 hash = serializer.read<uint64>();
			const std::string_view stringView = serializer.readStringView(0xffff);
			set(hash, stringView);
		}
		else
		{
			serializer.write(getHash());
			serializer.write(getString(), 0xffff);
		}
	}

	void FlyweightString::write(VectorBinarySerializer& serializer) const
	{
		serializer.write(getHash());
		serializer.write(getString(), 0xffff);
	}
}
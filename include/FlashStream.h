/*
Copyright 2016 Thomas Jammet
mathieu.poux[a]gmail.com
jammetthomas[a]gmail.com

This file is part of Librtmfp.

Librtmfp is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Librtmfp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Librtmfp.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Mona/Mona.h"
#include "Mona/Event.h"
#include "AMF.h"
#include "AMFReader.h"
#include "Mona/Packet.h"

/**************************************************************
FlashStream is linked to an as3 NetStream
*/
struct FlashStream : virtual Mona::Object {
	typedef Mona::Event<bool(const std::string& code, const std::string& description, Mona::UInt16 streamId, Mona::UInt64 flowId, double cbHandler)>	ON(Status); // NetConnection or NetStream status event
	typedef Mona::Event<void(const std::string& stream, Mona::UInt32 time, const Mona::Packet& packet, double lostRate, AMF::Type type)>				ON(Media);  // Received when we receive media (audio/video) in server or p2p 1-1
	typedef Mona::Event<bool(const std::string& streamName, Mona::UInt16 streamId, Mona::UInt64 flowId, double cbHandler)>								ON(Play); // Received when a peer is trying to play a stream
	typedef Mona::Event<void(const std::string& rawId, const std::string& peerId)>																		ON(NewPeer); // Received when the server send us a peer ID (after NetGroup connection)
	typedef Mona::Event<bool(const std::string& groupId, const std::string& key, const std::string& peerId)>											ON(GroupHandshake); // Received when a connected peer send us the Group hansdhake (only for P2PSession)
	typedef Mona::Event<bool(Mona::BinaryReader& reader, Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>							ON(GroupMedia); // Received when a connected peer send us a peer Group Media (Subscription/Infos)
	typedef Mona::Event<void(Mona::BinaryReader& reader, Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>							ON(GroupReport);
	typedef Mona::Event<void(Mona::BinaryReader& reader, Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>							ON(GroupPlayPush);
	typedef Mona::Event<void(Mona::BinaryReader& reader, Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>							ON(GroupPlayPull);
	typedef Mona::Event<void(Mona::BinaryReader& reader, Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>							ON(FragmentsMap);
	typedef Mona::Event<void(Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>														ON(GroupBegin);
	typedef Mona::Event<void(Mona::UInt8 type, Mona::UInt64 id, Mona::UInt8 splitNumber, Mona::UInt8 mediaType, Mona::UInt32 time, 
		const Mona::Packet& packet, double lostRate, Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>								ON(Fragment);
	typedef Mona::Event<bool(Mona::UInt16 streamId, Mona::UInt64 flowId, Mona::UInt64 writerId)>														ON(GroupAskClose); // Receiver when the peer want us to close the connection (if we accept we must close the current flow)

	FlashStream(Mona::UInt16 id);
	virtual ~FlashStream();

	const Mona::UInt16	id;

	Mona::UInt32	bufferTime(Mona::UInt32 ms);
	Mona::UInt32	bufferTime() const { return _bufferTime; }

	// return flase if writer is closed!
	virtual bool	process(const Mona::Packet& packet, Mona::UInt64 flowId, Mona::UInt64 writerId, double lostRate=0);

	virtual void	flush() { }

protected:
	virtual bool	messageHandler(const std::string& name, AMFReader& message, Mona::UInt64 flowId, Mona::UInt64 writerId, double callbackHandler);
	virtual bool	audioHandler(Mona::UInt32 time, const Mona::Packet& packet, double lostRate);
	virtual bool	videoHandler(Mona::UInt32 time, const Mona::Packet& packet, double lostRate);

	bool			process(AMF::Type type, Mona::UInt32 time, const Mona::Packet& packet, Mona::UInt64 flowId, Mona::UInt64 writerId, double lostRate);

private:
	virtual bool	rawHandler(Mona::UInt16 type, const Mona::Packet& packet);
	virtual bool	dataHandler(const Mona::Packet& packet, double lostRate);

	Mona::UInt32	_bufferTime;
	std::string		_streamName;
};

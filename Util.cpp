//------------------------------------------------------------------------------
/*
    This file is part of consensus-sim
    Copyright (c) 2013, Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include "Core.h"

void Message::addPositions(const std::map<int, NodeState>& updates)
{
    // add this information to our message
    for (auto const& update : updates)
    {
        int node_id = update.first;

        if (node_id != to_node)
        {
            std::map<int, NodeState>::iterator search = data.find(node_id);
            // We didn't know about this node, or the node has a 0 id so store
            // the state in the message. What's the significance of a 0 id???
            if (search == data.end() || node_id == 0)
            {
                data.insert(update);
            }
            else
            {
                // We already had data about this node going in this message
                auto& possibly_updated = update.second;
                auto& stored = search->second;

                if (possibly_updated.ts > stored.ts) // `ts` => timestamp
                {
                    stored.ts = possibly_updated.ts;
                    stored.state = possibly_updated.state;
                }
            }
        }
    }
}

void Message::subPositions(const std::map<int, NodeState>& received)
{
    // We received this information from this node, so no need to send it
    for (auto const& state : received)
    {
        int node_id = state.first;

        if (node_id != to_node)
        {
            std::map<int, NodeState>::iterator search = data.find(node_id);
            bool found = search != data.end();

            if (found && state.second.ts >= search->second.ts)
            {
                data.erase(search); // The node doesn't need the data
            }
        }
    }
}
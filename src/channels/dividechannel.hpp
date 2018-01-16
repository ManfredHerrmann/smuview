/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2018 Frank Stettner <frank-stettner@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CHANNELS_DIVIDECHANNEL_HPP
#define CHANNELS_DIVIDECHANNEL_HPP

#include <memory>
#include <vector>

#include <QObject>

#include "src/channels/basechannel.hpp"
#include "src/channels/mathchannel.hpp"

using std::shared_ptr;
using std::vector;

namespace sigrok {
class Quantity;
class QuantityFlag;
class Unit;
}

namespace sv {

namespace data {
class AnalogSignal;
}

namespace channels {

class DivideChannel : public MathChannel
{
	Q_OBJECT

public:
	DivideChannel(
		const sigrok::Quantity *sr_quantity,
		vector<const sigrok::QuantityFlag *> sr_quantity_flags,
		const sigrok::Unit *sr_unit,
		shared_ptr<data::AnalogSignal> dividend_signal,
		shared_ptr<data::AnalogSignal> divisor_signal,
		const QString device_name,
		const QString channel_group_name,
		double channel_start_timestamp);

private:
	shared_ptr<data::AnalogSignal> dividend_signal_;
	shared_ptr<data::AnalogSignal> divisor_signal_;
	size_t next_dividend_signal_pos_;
	size_t next_divisor_signal_pos_;

private Q_SLOTS:
	void on_sample_added();

};

} // namespace channels
} // namespace sv

#endif // CHANNELS_DIVIDECHANNEL_HPP
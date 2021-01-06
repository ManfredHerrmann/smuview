/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2018-2021 Frank Stettner <frank-stettner@gmx.net>
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
 */

#ifndef UI_DATA_UNITCOMBOBOX_HPP
#define UI_DATA_UNITCOMBOBOX_HPP

#include <QComboBox>
#include <QWidget>

#include "src/data/datautil.hpp"

namespace sv {
namespace ui {
namespace data {

class UnitComboBox : public QComboBox
{
	Q_OBJECT

public:
	explicit UnitComboBox(QWidget *parent = nullptr);

	void select_unit(sv::data::Unit unit);
	sv::data::Unit selected_unit();

private:
	void setup_ui();

};

} // namespace data
} // namespace ui
} // namespace sv

#endif // UI_DATA_UNITCOMBOBOX_HPP

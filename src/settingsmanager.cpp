/*
 * This file is part of the SmuView project.
 *
 * Copyright (C) 2020-2021 Frank Stettner <frank-stettner@gmx.net>
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

#include <algorithm>
#include <memory>
#include <string>

#include <QDebug>
#include <QSettings>

#include "settingsmanager.hpp"
#include "src/session.hpp"
#include "src/channels/basechannel.hpp"
#include "src/data/properties/baseproperty.hpp"
#include "src/data/basesignal.hpp"
#include "src/devices/basedevice.hpp"
#include "src/devices/configurable.hpp"
#include "src/devices/deviceutil.hpp"

using std::shared_ptr;
using std::string;
using sv::devices::DeviceType;

namespace sv {

bool SettingsManager::restore_settings_ = true;

SettingsManager::SettingsManager()
{
}

string SettingsManager::format_key(const string &str)
{
	string str_formated = str;
	std::replace(str_formated.begin(), str_formated.end(), ':', '_');
	std::replace(str_formated.begin(), str_formated.end(), '/', '_');
	std::replace(str_formated.begin(), str_formated.end(), '\\', '_');
	return str_formated;
}

bool SettingsManager::restore_settings()
{
	return SettingsManager::restore_settings_;
}

void SettingsManager::set_restore_settings(bool restore_settings)
{
	SettingsManager::restore_settings_ = restore_settings;
}

bool SettingsManager::has_device_settings(
	shared_ptr<devices::BaseDevice> device)
{
	QSettings settings;
	return settings.childGroups().contains(device->settings_id());
}

void SettingsManager::save_configurable(
	const shared_ptr<devices::Configurable> &configurable,
	QSettings &settings, shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	if (origin_device == nullptr ||
			origin_device->settings_id() != configurable->device_settings_id()) {
		// Only store the device value if this specific configurable belongs to
		// an other device than the tab-/device-setting (origin_device) it is
		// stored into, belongs to (>= 0.0.5).
		settings.setValue(key_prefix + "device",
			QVariant(configurable->device_settings_id()));
	}
	settings.setValue(key_prefix + "configurable",
		QVariant(QString::fromStdString(configurable->name())));
}

void SettingsManager::save_channel(
	const shared_ptr<channels::BaseChannel> &channel,
	QSettings &settings, shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	if (origin_device == nullptr ||
			origin_device->settings_id() != channel->parent_device()->settings_id()) {
		// Only store the device value if this specific channel belongs to
		// an other device than the tab-/device-setting (origin_device) it is
		// stored into, belongs to (>= 0.0.5).
		settings.setValue(key_prefix + "device",
			QVariant(channel->parent_device()->settings_id()));
	}
	settings.setValue(key_prefix + "channel",
		QVariant(QString::fromStdString(channel->name())));
}

void SettingsManager::save_signal(
	const shared_ptr<data::BaseSignal> &signal,
	QSettings &settings, shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	SettingsManager::save_channel(
		signal->parent_channel(), settings, origin_device, key_prefix);

	settings.setValue(key_prefix + "signal_sr_q",
		data::datautil::get_sr_quantity_id(signal->quantity()));
	settings.setValue(key_prefix + "signal_sr_qf", QVariant::fromValue<uint64_t>(
		data::datautil::get_sr_quantity_flags_id(signal->quantity_flags())));
}

void SettingsManager::save_property(
	const shared_ptr<data::properties::BaseProperty> &property,
	QSettings &settings, shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	SettingsManager::save_configurable(
		property->configurable(), settings, origin_device, key_prefix);

	settings.setValue(key_prefix + "property_sr_type",
		devices::deviceutil::get_sr_config_key(property->config_key())->
			data_type()->id());
	settings.setValue(key_prefix + "property_sr_ck",
		devices::deviceutil::get_sr_config_key_id(property->config_key()));
}

std::shared_ptr< sv::devices::BaseDevice > sv::SettingsManager::restore_device(
	Session& session, QSettings& settings,
	std::shared_ptr< sv::devices::BaseDevice > origin_device,
	const QString &key_prefix)
{
	QString device_key = key_prefix + "device";

	if (!settings.contains(device_key)) {
		// Use the supplied origin_device here when no device (key) is stored in
		// this setting (>= 0.0.5).
		return origin_device;
	}
	if (origin_device != nullptr && origin_device->type() == DeviceType::DemoDev) {
		// This is a fallback for older settings (pre 0.0.5):
		return origin_device;
	}

	// If a device (key) is stored in the settings, it means, that this device
	// differs from the device (origin_device) this tab belongs to.
	string device_id = settings.value(device_key).toString().toStdString();
	if (session.device_map().count(device_id) == 0)
		return nullptr;

	return session.device_map()[device_id];
}

shared_ptr<devices::Configurable> SettingsManager::restore_configurable(
	Session &session, QSettings &settings,
	shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	QString configurable_key = key_prefix + "configurable";

	auto device = SettingsManager::restore_device(
		session, settings, origin_device, key_prefix);
	if (!device)
		return nullptr;

	if (!settings.contains(configurable_key))
		return nullptr;

	string conf_id = settings.value(configurable_key).toString().toStdString();
	if (device->configurable_map().count(conf_id) == 0)
		return nullptr;

	return device->configurable_map()[conf_id];
}

shared_ptr<data::properties::BaseProperty> SettingsManager::restore_property(
	Session &session, QSettings &settings,
	shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	QString property_key = key_prefix + "property";

	auto configurable = SettingsManager::restore_configurable(
		session, settings, origin_device, key_prefix);
	if (!configurable)
		return nullptr;

	if (!settings.contains(property_key+"_sr_type") ||
			!settings.contains(property_key+"_sr_ck"))
		return nullptr;

	//auto sr_type = settings.value(property_key+"_sr_type").value<int>(); // TODO
	auto sr_ck = settings.value(property_key+"_sr_ck").value<uint32_t>();
	auto ck = devices::deviceutil::get_config_key(sr_ck);
	if (configurable->property_map().count(ck) == 0)
		return nullptr;

	return configurable->property_map()[ck];
}

shared_ptr<channels::BaseChannel> SettingsManager::restore_channel(
	Session &session, QSettings &settings,
	shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	QString channel_key = key_prefix + "channel";

	auto device = SettingsManager::restore_device(
		session, settings, origin_device, key_prefix);
	if (!device)
		return nullptr;

	if (!settings.contains(channel_key))
		return nullptr;

	string channel_id = settings.value(channel_key).toString().toStdString();
	if (device->channel_map().count(channel_id) == 0)
		return nullptr;

	return device->channel_map()[channel_id];
}

shared_ptr<data::BaseSignal> SettingsManager::restore_signal(
	Session &session, QSettings &settings,
	shared_ptr<sv::devices::BaseDevice> origin_device,
	const QString &key_prefix)
{
	QString signal_key = key_prefix + "signal";

	auto channel = SettingsManager::restore_channel(
		session, settings, origin_device, key_prefix);
	if (!channel)
		return nullptr;

	if (!settings.contains(signal_key+"_sr_q") ||
			!settings.contains(signal_key+"_sr_qf"))
		return nullptr;

	auto sr_q = settings.value(signal_key+"_sr_q").value<uint32_t>();
	auto sr_qf = settings.value(signal_key+"_sr_qf").value<uint64_t>();
	auto mq = make_pair(
		data::datautil::get_quantity(sr_q),
		data::datautil::get_quantity_flags(sr_qf));
	if (channel->signal_map().count(mq) == 0)
		return nullptr;

	return channel->signal_map()[mq][0];
}

} // namespace sv

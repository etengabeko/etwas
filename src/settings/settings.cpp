#include "settings.h"
#include "settings_private.h"

namespace settings
{

Settings::Settings(const QString& fileName) :
    m_pimpl(new details::SettingsPrivate(fileName))
{

}

Settings::~Settings() NOEXCEPT
{
    m_pimpl.reset();
}

} // settings

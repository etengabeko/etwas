#include "settings_private.h"

namespace settings
{
namespace details
{

SettingsPrivate::SettingsPrivate(const QString& fileName) :
    m_fileName(fileName)
{
    Q_ASSERT_X(!fileName.isEmpty(), "SettingsPrivate::SettingsPrivate()", "Empty settings filename");
}

SettingsPrivate::~SettingsPrivate() NOEXCEPT
{

}

} // details
} // settings

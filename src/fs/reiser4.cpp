/*************************************************************************
 *  Copyright (C) 2008 by Volker Lanz <vl@fidra.de>                      *
 *  Copyright (C) 2016 by Andrius Štikonas <andrius@stikonas.eu>         *
 *                                                                       *
 *  This program is free software; you can redistribute it and/or        *
 *  modify it under the terms of the GNU General Public License as       *
 *  published by the Free Software Foundation; either version 3 of       *
 *  the License, or (at your option) any later version.                  *
 *                                                                       *
 *  This program is distributed in the hope that it will be useful,      *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *  GNU General Public License for more details.                         *
 *                                                                       *
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 *************************************************************************/

#include "fs/reiser4.h"

#include "util/capacity.h"
#include "util/externalcommand.h"

#include <QRegularExpression>
#include <QStringList>

namespace FS
{
FileSystem::CommandSupportType reiser4::m_GetUsed = FileSystem::cmdSupportNone;
FileSystem::CommandSupportType reiser4::m_GetLabel = FileSystem::cmdSupportNone;
FileSystem::CommandSupportType reiser4::m_Create = FileSystem::cmdSupportNone;
FileSystem::CommandSupportType reiser4::m_Move = FileSystem::cmdSupportNone;
FileSystem::CommandSupportType reiser4::m_Check = FileSystem::cmdSupportNone;
FileSystem::CommandSupportType reiser4::m_Copy = FileSystem::cmdSupportNone;
FileSystem::CommandSupportType reiser4::m_Backup = FileSystem::cmdSupportNone;

reiser4::reiser4(qint64 firstsector, qint64 lastsector, qint64 sectorsused, const QString& label) :
    FileSystem(firstsector, lastsector, sectorsused, label, FileSystem::Type::Reiser4)
{
}

void reiser4::init()
{
    m_GetLabel = cmdSupportCore;
    m_GetUsed = findExternal(QStringLiteral("debugfs.reiser4"), {}, 16) ? cmdSupportFileSystem : cmdSupportNone;
    m_Create = findExternal(QStringLiteral("mkfs.reiser4"), {}, 16) ? cmdSupportFileSystem : cmdSupportNone;
    m_Check = findExternal(QStringLiteral("fsck.reiser4"), {}, 16) ? cmdSupportFileSystem : cmdSupportNone;
    m_Move = m_Copy = (m_Check != cmdSupportNone) ? cmdSupportCore : cmdSupportNone;
    m_Backup = cmdSupportCore;
}

bool reiser4::supportToolFound() const
{
    return
        m_GetUsed != cmdSupportNone &&
        m_GetLabel != cmdSupportNone &&
//          m_SetLabel != cmdSupportNone &&
        m_Create != cmdSupportNone &&
        m_Check != cmdSupportNone &&
//          m_UpdateUUID != cmdSupportNone &&
//          m_Grow != cmdSupportNone &&
//          m_Shrink != cmdSupportNone &&
        m_Copy != cmdSupportNone &&
        m_Move != cmdSupportNone &&
        m_Backup != cmdSupportNone;
//          m_GetUUID != cmdSupportNone;
}

FileSystem::SupportTool reiser4::supportToolName() const
{
    return SupportTool(QStringLiteral("reiser4progs"), QUrl(QStringLiteral("http://www.kernel.org/pub/linux/utils/fs/reiser4/reiser4progs/")));
}

qint64 reiser4::maxCapacity() const
{
    // looks like it's actually unknown. see
    // http://en.wikipedia.org/wiki/Comparison_of_file_systems
    return Capacity::unitFactor(Capacity::Unit::Byte, Capacity::Unit::EiB);
}

int reiser4::maxLabelLength() const
{
    return 16;
}

qint64 reiser4::readUsedCapacity(const QString& deviceNode) const
{
    ExternalCommand cmd(QStringLiteral("debugfs.reiser4"), { deviceNode });

    if (cmd.run(-1) && cmd.exitCode() == 16) {
        qint64 blocks = -1;
        QRegularExpression re(QStringLiteral("blocks:\\s+(\\d+)"));
        QRegularExpressionMatch reBlocks = re.match(cmd.output());

        if (reBlocks.hasMatch())
            blocks = reBlocks.captured(1).toLongLong();

        qint64 blockSize = -1;
        re.setPattern(QStringLiteral("blksize:\\s+(\\d+)"));
        QRegularExpressionMatch reBlockSize = re.match(cmd.output());

        if (reBlockSize.hasMatch())
            blockSize = reBlockSize.captured(1).toLongLong();

        qint64 freeBlocks = -1;
        re.setPattern(QStringLiteral("free blocks:\\s+(\\d+)"));
        QRegularExpressionMatch reFreeBlocks = re.match(cmd.output());

        if (reFreeBlocks.hasMatch())
            freeBlocks = reFreeBlocks.captured(1).toLongLong();

        if (blocks > - 1 && blockSize > -1 && freeBlocks > -1)
            return (blocks - freeBlocks) * blockSize;
    }

    return -1;
}

bool reiser4::check(Report& report, const QString& deviceNode) const
{
    ExternalCommand cmd(report, QStringLiteral("fsck.reiser4"), { QStringLiteral("--yes"), QStringLiteral("--fix"), deviceNode });
    return cmd.run(-1) && cmd.exitCode() == 0;
}

bool reiser4::create(Report& report, const QString& deviceNode)
{
    ExternalCommand cmd(report, QStringLiteral("mkfs.reiser4"), { QStringLiteral("--yes"), QStringLiteral("--force"), deviceNode });
    return cmd.run(-1) && cmd.exitCode() == 0;
}
}

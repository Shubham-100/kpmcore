/*************************************************************************
 *  Copyright (C) 2008,2009 by Volker Lanz <vl@fidra.de>                 *
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

#if !defined(EXT2__H)

#define EXT2__H

#include "util/libpartitionmanagerexport.h"

#include "fs/filesystem.h"

#include <QtGlobal>

class Report;

class QString;

namespace FS
{
/** An ext2 file system.
    @author Volker Lanz <vl@fidra.de>
*/
class LIBKPMCORE_EXPORT ext2 : public FileSystem
{
public:
    ext2(qint64 firstsector, qint64 lastsector, qint64 sectorsused, const QString& label, FileSystem::Type t = FileSystem::Ext2);

public:
    static void init();

    virtual qint64 readUsedCapacity(const QString& deviceNode) const override;
    virtual bool check(Report& report, const QString& deviceNode) const override;
    virtual bool create(Report& report, const QString& deviceNode) const override;
    virtual bool resize(Report& report, const QString& deviceNode, qint64 length) const override;
    virtual bool writeLabel(Report& report, const QString& deviceNode, const QString& newLabel) override;
    virtual bool updateUUID(Report& report, const QString& deviceNode) const override;

    virtual CommandSupportType supportGetUsed() const override {
        return m_GetUsed;
    }
    virtual CommandSupportType supportGetLabel() const override {
        return m_GetLabel;
    }
    virtual CommandSupportType supportCreate() const override {
        return m_Create;
    }
    virtual CommandSupportType supportGrow() const override {
        return m_Grow;
    }
    virtual CommandSupportType supportShrink() const override {
        return m_Shrink;
    }
    virtual CommandSupportType supportMove() const override {
        return m_Move;
    }
    virtual CommandSupportType supportCheck() const override {
        return m_Check;
    }
    virtual CommandSupportType supportCopy() const override {
        return m_Copy;
    }
    virtual CommandSupportType supportBackup() const override {
        return m_Backup;
    }
    virtual CommandSupportType supportSetLabel() const override {
        return m_SetLabel;
    }
    virtual CommandSupportType supportUpdateUUID() const override {
        return m_UpdateUUID;
    }
    virtual CommandSupportType supportGetUUID() const override {
        return m_GetUUID;
    }

    virtual qint64 maxCapacity() const override;
    virtual qint64 maxLabelLength() const override;
    virtual SupportTool supportToolName() const override;
    virtual bool supportToolFound() const override;

public:
    static CommandSupportType m_GetUsed;
    static CommandSupportType m_GetLabel;
    static CommandSupportType m_Create;
    static CommandSupportType m_Grow;
    static CommandSupportType m_Shrink;
    static CommandSupportType m_Move;
    static CommandSupportType m_Check;
    static CommandSupportType m_Copy;
    static CommandSupportType m_Backup;
    static CommandSupportType m_SetLabel;
    static CommandSupportType m_UpdateUUID;
    static CommandSupportType m_GetUUID;
};
}

#endif

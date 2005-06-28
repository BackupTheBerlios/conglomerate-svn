///////////////////////////////////////////////////////////////////////////////
///  @file OgreAssetManager.h
///  @brief <TODO: insert file description here>
///
///  @author jacmoe @date 28-06-2005
///  
///////////////////////////////////////////////////////////////////////////////
///  
///  This file is part of OgreOpcode.
///  
///  OgreOpcode is free software; you can redistribute it and/or
///  modify it under the terms of the GNU Lesser General Public
///  License as published by the Free Software Foundation; either
///  version 2.1 of the License, or (at your option) any later version.
///  
///  OgreOpcode is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
///  Lesser General Public License for more details.
///  
///  You should have received a copy of the GNU Lesser General Public
///  License along with OgreOpcode; if not, write to the Free Software
///  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///  
///////////////////////////////////////////////////////////////////////////////
#ifndef __OgreAssetManager_h__
#define __OgreAssetManager_h__

#include "OgreOpcodeExports.h"
#include <map>
#include <list>

namespace OgreOpcode
{
   namespace Details
   {
      class MemoryAsset;

      /// A class to manage heap memory objects
      class MemoryAssetManager : public Singleton<MemoryAssetManager>
      {
         friend class MemoryAsset;
         typedef std::list<MemoryAsset*> MemoryAssetList;
         typedef std::map<String, MemoryAssetList> MemoryAssetDatabase;

      public:
         ~MemoryAssetManager();

         static MemoryAssetManager& getSingleton(void);

         /// Returns the number of assets in a group
         int GetAssetCount(const String group) const;
         
         /// Returns the number of groups
         int GetGroupCount() const;

         /// Returns a specific asset
         MemoryAsset* GetAsset(const String group, const unsigned int asset) const;

         /// Returns a specific asset
         MemoryAsset* GetAsset(const String group, const String name) const;

         /// Deletes all assets in a group
         void DeleteAll(const String group);

         /// Deletes all assets
         void DeleteAll();

      protected:
         MemoryAssetManager();

         /// Adds an asset to the database
         void AddAsset(const String group, MemoryAsset *asset);

         /// Removes an asset from the database
         void RemoveAsset(const String group, MemoryAsset *asset);

         /// the list of all assets
         MemoryAssetDatabase database;
      };

      class MemoryAsset
      {
      public:
         MemoryAsset(String group, String name)
         {
            MemoryAssetManager::getSingleton().AddAsset(group, this);
            this->name = name;
            this->group = group;
         }
         virtual ~MemoryAsset()
         {
            MemoryAssetManager::getSingleton().RemoveAsset(group, this);
         }

         /// Returns the name of this object
         virtual String GetName() const {return name;}

         /// Returns the group of this object
         virtual String GetGroup() const {return group;}

      protected:
         /// the objects name
         String name;
         /// the objects group
         String group;
      };

   } // namespace Details
} // namespace OgreOpcode

#endif // __OgreAssetManager_h__
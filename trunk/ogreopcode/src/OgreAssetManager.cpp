///////////////////////////////////////////////////////////////////////////////
///  @file OgreAssetManager.cpp
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
#include "OgreAssetManager.h"

#include <assert.h>

template<> OgreOpcode::Details::MemoryAssetManager* Ogre::Singleton<OgreOpcode::Details::MemoryAssetManager>::ms_Singleton = 0;

namespace OgreOpcode
{
   namespace Details
   {

      MemoryAssetManager& MemoryAssetManager::getSingleton(void)
      {
         return Singleton<MemoryAssetManager>::getSingleton();
      }

      MemoryAssetManager::MemoryAssetManager()
      {

      }

      MemoryAssetManager::~MemoryAssetManager()
      {
         DeleteAll();
      }

      int MemoryAssetManager::GetAssetCount(const String group) const 
      {
         MemoryAssetDatabase::const_iterator iter = database.find(group);
         if (iter == database.end())
            return 0;

         return (int)iter->second.size();
      }

      int MemoryAssetManager::GetGroupCount() const
      {
         return (int)database.size();
      }

      MemoryAsset* MemoryAssetManager::GetAsset(const String group, const unsigned int asset) const
      {
         MemoryAssetDatabase::const_iterator iter = database.find(group);
         if (iter == database.end())
            return NULL;

         if (iter->second.size() < asset)
            return NULL;

         int i(0);
         for (MemoryAssetList::const_iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
         {
            if (i == asset)
               return *iter2;

            ++i;
         }

         return NULL;
      }

      MemoryAsset* MemoryAssetManager::GetAsset(const String group, const String name) const
      {
         MemoryAssetDatabase::const_iterator iter = database.find(group);
         if (iter == database.end())
            return NULL;

         for (MemoryAssetList::const_iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
         {
            assert(*iter2);
            if ((*iter2)->GetName() == name)
               return (*iter2);
         }

         return NULL;
      }

      void MemoryAssetManager::AddAsset(const String group, MemoryAsset *asset)
      {
         database[group].push_back(asset);
      }

      void MemoryAssetManager::RemoveAsset(const String group, MemoryAsset *asset)
      {
         MemoryAssetDatabase::iterator iter = database.find(group);
         if (iter != database.end())
         {
            iter->second.remove(asset);
         }
      }

      void MemoryAssetManager::DeleteAll(const String group)
      {
         MemoryAssetDatabase::iterator iter = database.find(group);
         if (iter != database.end())
         {
            while (iter->second.size() != 0)
               delete (*iter->second.begin());

            database.erase(iter);
         }	
      }

      void MemoryAssetManager::DeleteAll()
      {
         for (MemoryAssetDatabase::const_iterator iter = database.begin(); iter != database.end(); ++iter)
         {
            while (iter->second.size() != 0)
               delete (*iter->second.begin());
         }

         database.clear();
      }

   } // namespace Details
} // namespace OgreOpcode

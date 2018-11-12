class AuthPlayer
{
    const string AUTH_DIRECTORY = "$profile:Players\\";
    const string FILE_TYPE = "";

    ref Permission RootPermission;

    protected Man m_Object;

    protected ref PlayerData m_Data;

    void AuthPlayer( PlayerData data )
    {
        m_Object = NULL;

        m_Data = data;

        RootPermission = new ref Permission( m_Data.SGUID, NULL );

        if ( GetGame().IsServer() )
        {
            MakeDirectory( AUTH_DIRECTORY );
        }
    }

    void ~AuthPlayer()
    {
        delete RootPermission;
    }

    ref PlayerData GetData()
    {
        return m_Data;
    }  

    string GetGUID()
    {
        return m_Data.SGUID;
    }

    string GetName()
    {
        return m_Data.SName;
    }

    ref Man GetPlayerObject()
    {
        return m_Object;
    }

    void UpdatePlayerObject( ref Man obj )
    {
        m_Object = obj;
    }

    void CopyPermissions( ref Permission copy )
    {
        ref array< string > data = new ref array< string >;
        copy.Serialize( data );

        for ( int i = 0; i < data.Count(); i++ )
        {
            AddPermission( data[i] );
        }
    }

    void ClearPermissions()
    {
        delete RootPermission;

        RootPermission = new ref Permission( m_Data.SGUID, NULL );
    }

    void AddPermission( string permission, PermissionType type = PermissionType.INHERIT )
    {
        RootPermission.AddPermission( permission, type );
    }

    bool HasPermission( string permission )
    {
        return RootPermission.HasPermission( permission );
    }

    ref array< string > Serialize()
    {
        ref array< string > data = new ref array< string >;
        RootPermission.Serialize( data );

        m_Data.APermissions = data;

        return data;
    }

    void Deserialize()
    {
        for ( int i = 0; i < m_Data.APermissions.Count(); i++ )
        {
            AddPermission( m_Data.APermissions[i] );
        }
    }

    bool Save()
    {
        FileHandle file = OpenFile( AUTH_DIRECTORY + m_Data.SGUID + FILE_TYPE, FileMode.WRITE );
            
        ref array< string > data = Serialize();

        if ( file != 0 )
        {
            string line;

            for ( int i = 0; i < data.Count(); i++ )
            {
                FPrintln( file, data[i] );
            }
            
            CloseFile(file);
            
            Print("Wrote to the players");
            return true;
        } else
        {
            Print("Failed to open the file for the player for writing.");
            return false;
        }
    }

    bool Load()
    {
        FileHandle file = OpenFile( AUTH_DIRECTORY + m_Data.SGUID + FILE_TYPE, FileMode.READ );
            
        ref array< string > data = new ref array< string >;

        if ( file != 0 )
        {
            string line;

            while ( FGets( file, line ) > 0 )
            {
                data.Insert( line );
            }

            CloseFile( file );
        } else
        {
            Print( "Failed to open the file for the player to read. Attemping to create." );

            file = OpenFile( AUTH_DIRECTORY + m_Data.SGUID + FILE_TYPE, FileMode.WRITE );

            if ( file != 0 )
            {
                CloseFile( file );
                Print( "Created the file." );
            } else
            {
                Print( "Failed to create the file." );
            }
            return false;
        }
        
        for ( int i = 0; i < data.Count(); i++ )
        {
            AddPermission( data[i] );
        }

        return true;
    }

    void DebugPrint()
    {
        Print( "Printing permissions for " + m_Data.SGUID );

        RootPermission.DebugPrint( 0 );
    }
}
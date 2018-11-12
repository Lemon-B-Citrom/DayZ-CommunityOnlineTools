class ObjectMenu extends Form
{
    protected TextListboxWidget m_classList;
    protected EditBoxWidget m_SearchBox;
    protected ButtonWidget m_btnSpawnGround;
    protected ButtonWidget m_btnSpawnCursor;
    protected ButtonWidget m_btnSpawnInventory;
    protected ButtonWidget m_btnCancel;
    protected EditBoxWidget m_QuantityItem;

    private ItemPreviewWidget m_item_widget;
    protected EntityAI previewItem;
    private int m_characterRotationX;
    private int m_characterRotationY; // Borrowed from inspectmenu
    private int m_characterScaleDelta;
    private vector m_characterOrientation;

    void ObjectMenu()
    {
    }

    void ~ObjectMenu()
    {
        if ( previewItem ) 
        {
            GetGame().ObjectDelete( previewItem );
        }
    }

    override string GetTitle()
    {
        return "Object Spawner";
    }
    
    override string GetIconName()
    {
        return "OB";
    }

    override bool ImageIsIcon()
    {
        return false;
    }

    void Init()
    {
        m_classList = TextListboxWidget.Cast( layoutRoot.FindAnyWidget( "classlist" ) );
        m_SearchBox = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "search_input" ) );
        m_btnSpawnGround = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_spawn_ground" ) );
        m_btnSpawnCursor = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_spawn_cursorpos" ) );
        m_btnSpawnInventory = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_spawn_inventory" ) );
        m_btnCancel = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "btn_cancel" ) );

        m_QuantityItem = EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "quantity_items" ) );
    }

    override void OnShow()
    {
        super.OnShow();
        UpdateList( "All" );
    }

    override void OnHide() 
    {
        super.OnHide();
        if ( previewItem ) 
        {
            GetGame().ObjectDelete( previewItem );
        }
    }

    override bool OnChange( Widget w, int x, int y, bool finished )
    {
        if ( w == m_SearchBox )
        {
            UpdateList( "All" );
            return true;
        }

        return false;
    }

    bool OnMouseEnter( Widget w , int x, int y ) 
    {
        if ( w == m_SearchBox ) 
        {
            GetPlayer().GetInputController().OverrideMovementSpeed( true, 0 );
        }
        return false;
    }

    bool OnMouseLeave( Widget w, Widget enterW, int x, int y ) 
    {
        if ( w == m_SearchBox ) 
        {
            GetPlayer().GetInputController().OverrideMovementSpeed( false, 0 );
        }
        return false;
    }

    override bool OnClick( Widget w, int x, int y, int button )
    {
        string strSelection = GetCurrentSelection();
        bool ai = false;

        int quantity = 0;
        string text = "";
        ItemBase oItem = NULL;

        if ( w.GetName() == "dump_selects" ) 
        {
            if ( previewItem ) 
            {
                string toCopy = "";
                TStringArray strings = new TStringArray;
                previewItem.GetSelectionList(strings);
                foreach(string selection : strings ) 
                {
                    vector pos = GetGame().ObjectGetSelectionPosition( previewItem, selection );
                    toCopy = toCopy + selection + " " + pos.ToString() + "\n";
                }
                GetGame().CopyToClipboard( toCopy );
                Message("Dumped selections to clipboard"); 
            }
        }

        if( strSelection != "" )
        {
            strSelection.ToLower();

            if ( GetGame().IsKindOf( strSelection, "DZ_LightAI" ) ) 
            {
                ai = true;
            }

            if( w == m_btnSpawnCursor )
            {
                GetRPCManager().SendRPC( "COS_Object", "SpawnObjectPosition", new Param3< string, vector, string >( strSelection, GetCursorPos(), m_QuantityItem.GetText() ), true, NULL );
            }
            else if ( w == m_btnSpawnGround )
            {
                GetRPCManager().SendRPC( "COS_Object", "SpawnObjectPosition", new Param3< string, vector, string >( strSelection, GetGame().GetPlayer().GetPosition(), m_QuantityItem.GetText() ), true );
            }
            else if ( w == m_btnSpawnInventory )
            {
                if ( GetSelectedPlayers().Count() )
                {
                    GetRPCManager().SendRPC( "COS_Object", "SpawnObjectInventory", new Param3< string, string, ref array< ref PlayerData > >( strSelection, m_QuantityItem.GetText(), SerializePlayers( GetSelectedPlayers() ) ), true );
                }
            }
        }

        if ( w.GetName().Contains( "btn_filter" ) ) 
        {
            string buttonName = w.GetName();
            buttonName.Replace("btn_filter_", "");
            UpdateList( buttonName );

            return true;
        }

        return false;
    }

    override bool OnItemSelected( Widget w, int x, int y, int row, int column, int oldRow, int oldColumn )
    {
        if ( w == m_classList ) 
        {
            string strSelection = GetCurrentSelection();
            m_characterOrientation = vector.Zero;

            if ( !m_item_widget )
            {
                Widget preview_panel = layoutRoot.FindAnyWidget("preview_pnl");

                if ( preview_panel ) 
                {
                    float width;
                    float height;
                    preview_panel.GetSize(width, height);

                    m_item_widget = ItemPreviewWidget.Cast( GetGame().GetWorkspace().CreateWidget(ItemPreviewWidgetTypeID, 0, 0, 1, 1, WidgetFlags.VISIBLE, ARGB(255, 255, 255, 255), 10, preview_panel) );
                }
            }

            if ( previewItem ) 
            {
                GetGame().ObjectDelete( previewItem );
            }

            previewItem = GetGame().CreateObject( strSelection, vector.Zero, false );

            m_item_widget.SetItem( previewItem );
            m_item_widget.SetModelPosition( Vector(0,0,0.5) );
            m_item_widget.SetModelOrientation( m_characterOrientation );

            float itemx, itemy;        
            m_item_widget.GetPos(itemx, itemy);

            m_item_widget.SetSize( 1.5, 1.5 );

            // align to center 
            m_item_widget.SetPos( -0.225, -0.225 );
        }

        return true;
    }

    override bool OnMouseButtonDown( Widget w, int x, int y, int button ) 
    {
        if (w == m_item_widget)
        {
            GetGame().GetDragQueue().Call(this, "UpdateRotation");
            g_Game.GetMousePos(m_characterRotationX, m_characterRotationY);
            return true;
        }
        return false;
    }

    override bool OnMouseWheel( Widget w, int x, int y, int wheel ) 
    {
        if ( w == m_item_widget )
        {
            GetGame().GetDragQueue().Call(this, "UpdateScale");
            m_characterScaleDelta = wheel ;
        }
        return false;
    }

    void UpdateScale(int mouse_x, int mouse_y, int wheel, bool is_dragging) // Borrowed from inspect menu
    {
        float w, h, x, y;        
        m_item_widget.GetPos(x, y);
        m_item_widget.GetSize(w,h);
        w = w + ( m_characterScaleDelta / 4);
        h = h + ( m_characterScaleDelta / 4 );
        if ( w > 0.5 && w < 4 )
        {
            m_item_widget.SetSize( w, h );
    
            // align to center 
            int screen_w, screen_h;
            GetScreenSize(screen_w, screen_h);
            float new_x = x - ( m_characterScaleDelta / 8 );
            float new_y = y - ( m_characterScaleDelta / 8 );
            m_item_widget.SetPos( new_x, new_y );
        }
    }

    void UpdateRotation(int mouse_x, int mouse_y, bool is_dragging) // Borrowed from inspect menu
    {
        vector o = m_characterOrientation;
        o[0] = o[0] + (m_characterRotationY - mouse_y);
        o[1] = o[1] - (m_characterRotationX - mouse_x);
        
        m_item_widget.SetModelOrientation( o );
        
        if (!is_dragging)
        {
            m_characterOrientation = o;
        }
    }

    void UpdateList( string classFilter ) // All default
    {
        m_classList.ClearItems();

        TStringArray configs = new TStringArray;
        configs.Insert( CFG_VEHICLESPATH );
        configs.Insert( CFG_WEAPONSPATH );
        configs.Insert( CFG_MAGAZINESPATH );

        string strSearch = m_SearchBox.GetText();

        strSearch.ToLower();

        for ( int nConfig = 0; nConfig < configs.Count(); ++nConfig )
        {
            string strConfigPath = configs.Get( nConfig );

            int nClasses = g_Game.ConfigGetChildrenCount( strConfigPath );

            for ( int nClass = 0; nClass < nClasses; ++nClass )
            {
                string strName;

                g_Game.ConfigGetChildName( strConfigPath, nClass, strName );

                int scope = g_Game.ConfigGetInt( strConfigPath + " " + strName + " scope" );

                if ( scope == 0 )
                {
                    continue;
                }

                string strNameLower = strName;

                strNameLower.ToLower();

                if ( GetGame().IsKindOf( strNameLower, classFilter ) || classFilter == "All" ) // Fix for weapon_base not being child of "All"
                {
                    if ( (strSearch != "" && (!strNameLower.Contains( strSearch ))) ) 
                    {
                        continue;
                    }

                    if ( strName == "ItemOptics" ) 
                    {
                        continue; // Fix crash
                    }

                    m_classList.AddItem( strName, NULL, 0 );
                }
            }
        }
    }

    string GetCurrentSelection()
    {
        if ( m_classList.GetSelectedRow() != -1 )
        {
            string result;
            m_classList.GetItemText( m_classList.GetSelectedRow(), 0, result );
            return result;
        }

        return "";
    }
}

// DZ_LightAI
// House?
// Transport
// Weapon_Base
// Edible_Base
// Clothing_Base
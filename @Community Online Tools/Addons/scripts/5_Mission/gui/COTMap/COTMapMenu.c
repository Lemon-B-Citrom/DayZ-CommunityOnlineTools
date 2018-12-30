class COTMapMenu extends Form 
{
	protected ref MapWidget map_widget;
	protected ref Widget basewindow_background;
	
    void COTMapMenu()
    {
    }

    void ~COTMapMenu()
    {
    }

    override string GetTitle()
    {
        return "Map";
    }
    
    override string GetIconName()
    {
        return "M";
    }
	
    override bool ImageIsIcon()
    {
        return false;
    }
	
	override void OnInit( bool fromMenu )
    {
        map_widget = MapWidget.Cast( layoutRoot.FindAnyWidget( "map_widget" ) );
		basewindow_background = window.GetLayoutRoot().FindAnyWidget("background");
		basewindow_background.Show(false);
    }
	
	override void OnShow()
    {
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( UpdatePlayers, 500, true );
    }

    override void OnHide() 
    {
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).Remove( UpdatePlayers );
    }

	void UpdatePlayers() 
	{
		GetRPCManager().SendRPC( "COT_Admin", "Request_Map_PlayerPositions", new Param, true );
	}
	
	void ShowPlayers( ref array<ref PlayerData> data ) 
	{
		map_widget.ClearUserMarks();
		
		foreach( ref PlayerData playerData : data ) 
		{
			map_widget.AddUserMark(playerData.VPosition, playerData.SName, ARGB(255, 0, 0, 0), "");
		}
	}
}
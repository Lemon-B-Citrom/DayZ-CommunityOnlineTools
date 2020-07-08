class JMWebhookConnection : Managed
{
    string Name;
    bool Enabled;
    
    [NonSerialized()]
    JMWebhookConnectionGroup m_Group;
    
    [NonSerialized()]
    RestContext m_Context;

    string GetContextURL()
    {
        return m_Group.ContextURL;
    }

    string GetAddress()
    {
        return m_Group.Address;
    }

    void Remove()
    {
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).Call( m_Group.Remove, Name );
    }

	void Post( RestApi core, notnull ref JsonSerializer serializer, notnull ref JMWebhookMessage message )
	{
        if ( Enabled && !IsMissionClient() )
        {
            if ( !m_Context )
            {
                m_Context = core.GetRestContext( GetContextURL() );
            }

            string data = message.Prepare( serializer );

            m_Context.SetHeader( "application/json" );
            m_Context.POST( new JMWebhookCallback, GetAddress(), data );
        }
	}

    string DebugString()
    {
        string str = "";
        if ( m_Context )
            str += "CTX CREATED";
        else
            str += "CTX NOT CREATED";

        str += ":";
        str += Name;
        str += ":";
        str += GetContextURL();
        str += ":";
        str += GetAddress();
        str += ":";
        if ( Enabled )
            str += "TRUE";
        else 
            str += "FALSE";

        return str;
    }
};
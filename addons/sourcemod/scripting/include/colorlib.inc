#if defined _colorlib_included
    #endinput
#endif
#define _colorlib_included

#include <colorlib_map>
 
#define MAX_MESSAGE_LENGTH 250

#define SERVER_INDEX 0
#define NO_INDEX -1
#define NO_PLAYER -2

#define CL_LoopClients(%1) for (int %1 = 1; %1 <= MaxClients; ++%1)
#define CL_CheckClientIndex(%1) if (%1 <= 0 || %1 > MaxClients) ThrowError("Invalid client index %d", %1)
#define CL_CheckClientIsIngame(%1) if (!IsClientInGame(%1)) ThrowError("Client %d is not in game", %1)

/* C_Colors' properties */
bool _CL_SkipList[MAXPLAYERS + 1] = { false, ... };

static Handle sm_show_activity = null;

/**
 * Prints a message to a specific client in the chat area.
 * Supports color tags.
 *
 * @param client	  Client index.
 * @param message   Message (formatting rules).
 * @return			  No return
 * 
 * On error/Errors:   If the client is not connected an error will be thrown.
 */
stock void CPrintToChat(int client, const char[] message, any ...)
{
    CL_CheckClientIndex(client);
    CL_CheckClientIsIngame(client);

    SetGlobalTransTarget(client);

    char buffer[MAX_MESSAGE_LENGTH];
    buffer[0] = 0x01;
    VFormat(buffer[1], sizeof(buffer) - 1, message, 3);

    int index = CFormat(buffer, sizeof(buffer));
    if (index == NO_INDEX)
        PrintToChat(client, "%s", buffer);
    else
        C_SayText2(client, index, buffer);
}

/**
 * Reples to a message in a command. A client index of 0 will use PrintToServer().
 * If the command was from the console, PrintToConsole() is used. If the command was from chat, C_PrintToChat() is used.
 * Supports color tags.
 *
 * @param client	  Client index, or 0 for server.
 * @param message   Formatting rules.
 * @param ...         Variable number of format parameters.
 * @return			  No return
 * 
 * On error/Errors:   If the client is not connected or invalid.
 */
stock void CReplyToCommand(int client, const char[] message, any ...)
{
    SetGlobalTransTarget(client);

    char buffer[MAX_MESSAGE_LENGTH];
    VFormat(buffer, sizeof(buffer), message, 3);

    if (client == 0)
    {
        CRemoveTags(buffer, sizeof(buffer));
        PrintToServer("%s", buffer);
    }
    else if (GetCmdReplySource() == SM_REPLY_TO_CONSOLE)
    {
        CRemoveTags(buffer, sizeof(buffer));
        PrintToConsole(client, "%s", buffer);
    }
    else
    {
        CPrintToChat(client, "%s", buffer);
    }
}

/**
 * Reples to a message in a command. A client index of 0 will use PrintToServer().
 * If the command was from the console, PrintToConsole() is used. If the command was from chat, C_PrintToChat() is used.
 * Supports color tags.
 *
 * @param client	  Client index, or 0 for server.
 * @param author      Author index whose color will be used for teamcolor tag.
 * @param message   Formatting rules.
 * @param ...         Variable number of format parameters.
 * @return			  No return
 * 
 * On error/Errors:   If the client is not connected or invalid.
 */
stock void CReplyToCommandEx(int client, int author, const char[] message, any ...)
{
    char buffer[MAX_MESSAGE_LENGTH];
    SetGlobalTransTarget(client);
    VFormat(buffer, sizeof(buffer), message, 4);

    if (client == 0)
    {
        CRemoveTags(buffer, sizeof(buffer));
        PrintToServer("%s", buffer);
    }
    else if (GetCmdReplySource() == SM_REPLY_TO_CONSOLE)
    {
        CRemoveTags(buffer, sizeof(buffer));
        PrintToConsole(client, "%s", buffer);
    }
    else
    {
        CPrintToChatEx(client, author, "%s", buffer);
    }
}

/**
 * Prints a message to all clients in the chat area.
 * Supports color tags.
 *
 * @param client	  Client index.
 * @param message   Message (formatting rules)
 * @return			  No return
 */
stock void CPrintToChatAll(const char[] message, any ...)
{
    char buffer[MAX_MESSAGE_LENGTH];

    CL_LoopClients(i)
    {
        if (i > 0 && IsClientInGame(i) && !IsFakeClient(i) && !_CL_SkipList[i])
        {
            SetGlobalTransTarget(i);
            VFormat(buffer, sizeof(buffer), message, 2);

            CPrintToChat(i, "%s", buffer);
        }

        _CL_SkipList[i] = false;
    }
}

/**
 * Prints a message to a specific client in the chat area.
 * Supports color tags and teamcolor tag.
 *
 * @param client	  Client index.
 * @param author	  Author index whose color will be used for teamcolor tag.
 * @param message   Message (formatting rules).
 * @return			  No return
 * 
 * On error/Errors:   If the client or author are not connected an error will be thrown.
 */
stock void CPrintToChatEx(int client, int author, const char[] message, any ...)
{
    CL_CheckClientIndex(client);
    CL_CheckClientIsIngame(client);
    CL_CheckClientIndex(author);

    char buffer[MAX_MESSAGE_LENGTH];
    SetGlobalTransTarget(client);

    buffer[0] = 0x01;
    VFormat(buffer[1], sizeof(buffer) - 1, message, 4);

    int index = CFormat(buffer, sizeof(buffer), author);
    if (index == NO_INDEX)
        PrintToChat(client, "%s", buffer);
    else
        C_SayText2(client, author, buffer);
}

/**
 * Prints a message to all clients in the chat area.
 * Supports color tags and teamcolor tag.
 *
 * @param author	  Author index whos color will be used for teamcolor tag.
 * @param message   Message (formatting rules).
 * @return			  No return
 * 
 * On error/Errors:   If the author is not connected an error will be thrown.
 */
stock void CPrintToChatAllEx(int author, const char[] message, any ...)
{
    CL_CheckClientIndex(author);
    CL_CheckClientIsIngame(author);

    char buffer[MAX_MESSAGE_LENGTH];

    CL_LoopClients(i)
    {
        if (i > 0 && IsClientInGame(i) && !IsFakeClient(i) && !_CL_SkipList[i])
        {
            SetGlobalTransTarget(i);
            VFormat(buffer, sizeof(buffer), message, 3);

            CPrintToChatEx(i, author, "%s", buffer);
        }

        _CL_SkipList[i] = false;
    }
}

stock void CPrintToServer(const char[] message, any ...)
{
    char buffer[MAX_MESSAGE_LENGTH];
    VFormat(buffer, sizeof(buffer), message, 2);

    CRemoveTags(buffer, sizeof(buffer));

    PrintToServer(buffer);
}

/**
 * Removes color tags from the string.
 *
 * @param message   String.
 * @return			  No return
 */
stock void CRemoveTags(char[] message, int maxlength)
{
    int index = 0;
    char buffer[MAX_MESSAGE_LENGTH];
    for (int i = 0; i < maxlength; ++i)
    {
        if (message[i] == '{')
        {
            ++i;
            while (message[i] != '}')
            {
                ++i;
            }
        }
        else
        {
            buffer[index] = message[i];
            ++index;
        }
    }

    strcopy(message, maxlength, buffer);
}

/**
 * This function should only be used right in front of
 * C_PrintToChatAll or C_PrintToChatAllEx and it tells
 * to those funcions to skip specified client when printing
 * message to all clients. After message is printed client will
 * no more be skipped.
 * 
 * @param client   Client index
 * @return		   No return
 */
stock void CSkipNextClient(int client)
{
    if (client <= 0 || client > MaxClients)
        ThrowError("Invalid client index %d", client);

    _CL_SkipList[client] = true;
}

/**
 * Replaces color tags in a string with color codes
 *
 * @param message   String.
 * @param maxlength   Maximum length of the string buffer.
 * @return			  Client index that can be used for SayText2 author index
 * 
 * On error/Errors:   If there is more then one team color is used an error will be thrown.
 */
stock int CFormat(char[] message, int maxlength, int author = NO_INDEX)
{
    int iRandomPlayer = NO_INDEX;

    int index = 0;
    char buffer[250];
    // If CS:GO set invisible precolor
    if (GetEngineVersion() == Engine_CSGO) 
    {
        buffer[0] = ' ';
        ++index;
    }

    for (int i = 0; i < maxlength; ++i)
    {
        if (message[i] == 0x00)
        {
            break;
        }

        if (message[i] == '\\')
        {
            ++i;
            if (message[i] == '{')
            {
                buffer[index] = '{';
            }
            else
            {
                buffer[index] = '\\';
                --i;
            }

            ++index;
        }
        else if (message[i] == '{')
        {
            ++i;
            char color[16];
            for (int icolor = 0; icolor < sizeof(color); ++icolor)
            {
                if (message[i] == '}')
                {
                    break;
                }

                color[icolor] = message[i];
                ++i;
            }

            PrintToServer(color);
            buffer[index] = view_as<char>(_CL_ColorMap(color));
            ++index;
        }
        else
        {
            buffer[index] = message[i];
            ++index;
        }
    }

    strcopy(message, maxlength, buffer);

    return iRandomPlayer;
}

/**
 * Sends a SayText2 usermessage to a client
 *
 * @param message   Client index
 * @param maxlength   Author index
 * @param message   Message
 * @return			  No return.
 */
stock void C_SayText2(int client, int author, const char[] message)
{
    Handle hBuffer = StartMessageOne("SayText2", client, USERMSG_RELIABLE|USERMSG_BLOCKHOOKS);

    if(GetFeatureStatus(FeatureType_Native, "GetUserMessageType") == FeatureStatus_Available && GetUserMessageType() == UM_Protobuf) 
    {
        PbSetInt(hBuffer, "ent_idx", author);
        PbSetBool(hBuffer, "chat", true);
        PbSetString(hBuffer, "msg_name", message);
        PbAddString(hBuffer, "params", "");
        PbAddString(hBuffer, "params", "");
        PbAddString(hBuffer, "params", "");
        PbAddString(hBuffer, "params", "");
    }
    else
    {
        BfWriteByte(hBuffer, author);
        BfWriteByte(hBuffer, true);
        BfWriteString(hBuffer, message);
    }

    EndMessage();
}
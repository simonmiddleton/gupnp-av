/*
 * Copyright (C) 2012 Intel Corporation
 *
 * Authors: Krzesimir Nowak <krnowak@openismus.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <glib-object.h>
#include <libgupnp-av/gupnp-didl-lite-object.h>
#include <libgupnp-av/gupnp-didl-lite-writer.h>
#include <libgupnp-av/gupnp-didl-lite-item.h>

/* creates an item described by:
static const gchar * const didllite =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<DIDL-Lite\n"
        "xmlns:dc=\"http://purl.org/dc/elements/1.1/\"\n"
        "xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\"\n"
        "xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\"\n"
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
        "xsi:schemaLocation=\"\n"
        "urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\n"
        "http://www.upnp.org/schemas/av/didl-lite.xsd\n"
        "urn:schemas-upnp-org:metadata-1-0/upnp/\n"
        "http://www.upnp.org/schemas/av/upnp.xsd\">\n"
        "<item id=\"18\" parentID=\"13\" restricted=\"0\">\n"
        "<dc:title>Try a little tenderness</dc:title>\n"
        "<upnp:class>object.item.audioItem.musicTrack</upnp:class>\n"
        "<res protocolInfo=\"http-get:*:audio/mpeg:*\" size=\"3558000\">\n"
        "http://168.192.1.1/audio197.mp3\n"
        "</res>\n"
        "<upnp:artist>Unknown</upnp:artist>\n"
        "</item>\n"
        "</DIDL-Lite>\n";
*/
static GUPnPDIDLLiteObject *
get_item (void)
{
        GUPnPDIDLLiteWriter *writer = gupnp_didl_lite_writer_new (NULL);
        GUPnPDIDLLiteItem *item = gupnp_didl_lite_writer_add_item (writer);
        GUPnPDIDLLiteObject *object = GUPNP_DIDL_LITE_OBJECT (item);
        GUPnPDIDLLiteContributor *artist;
        GUPnPDIDLLiteResource *resource;
        GUPnPProtocolInfo *info;

        g_object_unref (writer);
        gupnp_didl_lite_object_set_id (object, "18");
        gupnp_didl_lite_object_set_parent_id (object, "13");
        gupnp_didl_lite_object_set_restricted (object, FALSE);
        gupnp_didl_lite_object_set_title (object, "Try a little tenderness");
        gupnp_didl_lite_object_set_upnp_class
                                        (object,
                                         "object.item.audioItem.musicTrack");
        artist = gupnp_didl_lite_object_add_artist (object);
        gupnp_didl_lite_contributor_set_name (artist, "Unknown");
        g_object_unref (artist);
        resource = gupnp_didl_lite_object_add_resource (object);
        info = gupnp_protocol_info_new ();
        gupnp_protocol_info_set_protocol (info, "http-get");
        gupnp_protocol_info_set_network (info, "*");
        gupnp_protocol_info_set_mime_type (info, "audio/mpeg");
        gupnp_didl_lite_resource_set_protocol_info (resource, info);
        g_object_unref (info);
        gupnp_didl_lite_resource_set_size (resource, 3558000);
        gupnp_didl_lite_resource_set_uri (resource,
                                          "http://168.192.1.1/audio197.mp3");
        g_object_unref (resource);

        return object;
}

static const gchar * const current_fragments[] = {
        /* 1 */
        "<upnp:class>object.item.audioItem.musicTrack</upnp:class>",
        /* 2 */
        "",
        /* 3 */
        "<upnp:artist>Unknown</upnp:artist>",
        /* 4 */
        "<dc:title>Try a little tenderness</dc:title>"
};

static const gchar * const new_fragments[] = {
        /* 1 */
        "<upnp:class>object.item.audioItem.musicTrack</upnp:class>"
        "<upnp:genre>Obscure</upnp:genre>",
        /* 2 */
        "<upnp:genre>Even more obscure</upnp:genre>",
        /* 3 */
        "",
        /* 4 */
        "<dc:title>Cthulhu fhtagn</dc:title>"
};

static GList*
create_fragment_list (const gchar * const * const fragments,
                      guint count)
{
        GList *list = NULL;
        guint iter;

        for (iter = 0; iter < count; ++iter) {
                list = g_list_prepend (list, (gpointer) fragments[iter]);
        }

        return g_list_reverse (list);
}

static void
debug_dump (GUPnPDIDLLiteObject *object)
{
        xmlChar *dump = NULL;
        xmlNodePtr node = gupnp_didl_lite_object_get_xml_node (object);
        xmlDocPtr doc = node->doc;

        xmlDocDumpMemory (doc, &dump, NULL);
        g_debug ("Obj dump:\n%s", dump);
        xmlFree (dump);
}

int main (void)
{
        GUPnPDIDLLiteObject *object;
        GList *current = create_fragment_list
                                        (current_fragments,
                                         G_N_ELEMENTS (current_fragments));
        GList *new = create_fragment_list (new_fragments,
                                           G_N_ELEMENTS (new_fragments));
        GUPnPDIDLLiteFragmentResult result;

        g_type_init ();
        g_setenv ("GUPNP_AV_DATADIR", ABS_TOP_SRCDIR G_DIR_SEPARATOR_S "data", FALSE);

        object = get_item ();
        debug_dump (object);
        result = gupnp_didl_lite_object_apply_fragments (object, current, new);
        g_list_free (new);
        g_list_free (current);
        debug_dump (object);
        g_object_unref (object);
        if (result != GUPNP_DIDL_LITE_FRAGMENT_RESULT_OK)
                return 1;
        return 0;
}

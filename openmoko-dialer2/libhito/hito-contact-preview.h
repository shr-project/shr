#ifndef _HITO_CONTACT_PREVIEW
#define _HITO_CONTACT_PREVIEW

#include <gtk/gtkvbox.h>
#include <libebook/e-contact.h>

G_BEGIN_DECLS

#define HITO_TYPE_CONTACT_PREVIEW hito_contact_preview_get_type()

#define HITO_CONTACT_PREVIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  HITO_TYPE_CONTACT_PREVIEW, HitoContactPreview))

#define HITO_CONTACT_PREVIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  HITO_TYPE_CONTACT_PREVIEW, HitoContactPreviewClass))

#define HITO_IS_CONTACT_PREVIEW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  HITO_TYPE_CONTACT_PREVIEW))

#define HITO_IS_CONTACT_PREVIEW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  HITO_TYPE_CONTACT_PREVIEW))

#define HITO_CONTACT_PREVIEW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  HITO_TYPE_CONTACT_PREVIEW, HitoContactPreviewClass))

typedef struct {
  GtkVBox parent;
} HitoContactPreview;

typedef struct {
  GtkVBoxClass parent_class;
} HitoContactPreviewClass;

GType hito_contact_preview_get_type (void);

GtkWidget* hito_contact_preview_new (void);

void hito_contact_preview_set_contact (HitoContactPreview *preview, EContact *contact);

/* TODO: pass EBookView to update, or let the user of this handle that? */

G_END_DECLS

#endif /* _HITO_CONTACT_PREVIEW */

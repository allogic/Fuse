#ifndef ED_CATALOG_VIEW_H
#define ED_CATALOG_VIEW_H

ed_catalog_view_t *ed_catalog_view_create(eg_context_t *context, asset_type_t asset_type);
void ed_catalog_view_refresh(ed_catalog_view_t *catalog);
void ed_catalog_view_draw(ed_catalog_view_t *catalog);
void ed_catalog_view_destroy(ed_catalog_view_t *catalog);

#endif // ED_CATALOG_VIEW_H

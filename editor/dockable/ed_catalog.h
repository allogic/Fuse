#ifndef ED_CATALOG_H
#define ED_CATALOG_H

ed_catalog_t ed_catalog_create(context_t *context, ed_asset_type_t asset_type);
void ed_catalog_refresh(ed_catalog_t *catalog);
void ed_catalog_draw(ed_catalog_t *catalog);
void ed_catalog_destroy(ed_catalog_t *catalog);

#endif // ED_CATALOG_H

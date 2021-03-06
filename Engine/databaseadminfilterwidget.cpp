#include "databaseadminfilterwidget.h"
#include "ui_databaseadminfilterwidget.h"
#include "databaseadminfiltercreate.h"

DatabaseAdminFilterWidget::DatabaseAdminFilterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseAdminFilterWidget)
{
    ui->setupUi(this);
    Layout = new FlowLayout(ui->FilterContent);
    ui->FilterContent->setLayout(Layout);

}

DatabaseAdminFilterWidget::~DatabaseAdminFilterWidget()
{
    delete ui;
}

void DatabaseAdminFilterWidget::Clear()
{
    foreach(DatabaseAdminFilterItem * Item, Items.keys())
    {
        Item->deleteLater();
    }
    Items.clear();
}

void DatabaseAdminFilterWidget::SetColumns(QList<DatabaseColumn> Columns)
{
    this->Columns = Columns;
}


DatabaseFilters DatabaseAdminFilterWidget::GetFilters()
{
    DatabaseFilters res;
    res.IsNull = false;
    res.FilterList = Items.values();
    return res;
}

void DatabaseAdminFilterWidget::DeleteFilter()
{
    Items.remove(qobject_cast<DatabaseAdminFilterItem*>(sender()));
    sender()->deleteLater();

    emit FiltersChanged();
}

void DatabaseAdminFilterWidget::on_AddFilter_clicked()
{
    DatabaseAdminFilterCreate d;
    d.SetColumns(Columns);
    if(d.exec())
    {
        DatabaseAdminFilterItem * Item = new DatabaseAdminFilterItem(ui->FilterContent);
        Item->SetText(d.GetFilterText());
        connect(Item, SIGNAL(NeedDelete()),this,SLOT(DeleteFilter()));
        Items[Item] = d.GetFilter();
        Layout->addWidget(Item);

        emit FiltersChanged();
    }
}

void DatabaseAdminFilterWidget::on_DeleteAllFilters_clicked()
{
    Clear();
}

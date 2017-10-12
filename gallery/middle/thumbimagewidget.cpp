#include "thumbimagewidget.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QCheckBox>
#include <QInputDialog>

#include "thumbimageitem.h"
#include "global_value.h"
#include "cmessagebox.h"
#include <QDebug>

#ifdef DEVICE_EVB
int thumb_image_width = 280;
int bottom_widget_height = 130;
int button_width = 130;
int button_height = 60;

#else
int thumb_image_width = 110;
int bottom_widget_height = 70;
int button_width = 80;
int button_height = 40;

#endif

ThumbImageWidget::ThumbImageWidget(QWidget *parent):BaseWidget(parent),editMode(false)
{
    m_middleWidgets = (GalleryMiddleWidgets*)parent;
    initLayout();
    initConnection();
}

void ThumbImageWidget::initLayout()
{
    QVBoxLayout *mainLyout = new QVBoxLayout;

    // Layout of bottom control widgets.
    m_controlBottom = new QWidget(this);
    m_controlBottom->setStyleSheet("background:rgb(20,22,28)");
    m_controlBottom->setFixedHeight(bottom_widget_height);

    m_imageCountText = new QLabel(this);
    m_imageCountText->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    QFont font = m_imageCountText->font();
    font.setPixelSize(font_size_big);
    m_imageCountText->setFont(font);
    m_imageCountText->adjustSize();

    m_tipText = new QLabel(this);
    m_tipText->setFont(font);
    m_tipText->adjustSize();

    m_btnMode = new QPushButton(str_button_edit_image,this);
    m_btnMode->setStyleSheet("QPushButton{background:rgb(36,184,71);color:white;border-radius:5px}"
                             "QPushButton::hover{background:rgb(25,166,58)}"
                             "QPushButton::pressed{background:rgb(25,166,58)}");
    m_btnUpdate = new QPushButton(str_button_refresh_image,this);
    m_btnUpdate->setStyleSheet("QPushButton{background:rgb(36,184,71);color:white;border-radius:5px}"
                               "QPushButton::hover{background:rgb(25,166,58)}"
                               "QPushButton::pressed{background:rgb(25,166,58)}");

    m_btnMode->setFixedSize(button_width,button_height);
    m_btnUpdate->setFixedSize(button_width,button_height);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addSpacing(0);
    buttonLayout->addWidget(m_btnUpdate);
    buttonLayout->addWidget(m_btnMode);
    buttonLayout->setSpacing(20);
    buttonLayout->addSpacing(20);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(m_imageCountText,1);
    bottomLayout->addWidget(m_tipText,1);
    bottomLayout->addLayout(buttonLayout,1);

    m_controlBottom->setLayout(bottomLayout);

    // Layout of image thumb list.
    m_imageListWid = new BaseListWidget(this);
    m_imageListWid->setIconSize(QSize(thumb_image_width,thumb_image_width));
    m_imageListWid->setStyleSheet("QListWidget::item:selected{background: transparent;}");
    /* cancel the border in ListWidget */
    m_imageListWid->setFrameShape(QListWidget::NoFrame);
    /* set QListWidget be IconMode to show image thumnail and unable to drag */
    m_imageListWid->setViewMode(QListView::IconMode);
    m_imageListWid->setMovement(QListView::Static);
    m_imageListWid->setSpacing(10);
    m_imageListWid->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_imageListWid->setFocusPolicy(Qt::NoFocus);
    m_imageListWid->setStyleSheet("QListWidget::indicator{subcontrol-position:top;}"
                                  "QListWidget{background:transparent}");


    mainLyout->addWidget(m_imageListWid);
    mainLyout->addWidget(m_controlBottom);
    mainLyout->setContentsMargins(0,0,0,0);
    mainLyout->setSpacing(0);

    setLayout(mainLyout);
}

void ThumbImageWidget::initConnection()
{
    connect(this,SIGNAL(imagesResChanged()),this,SLOT(slot_onImagesResChanged()));
    connect(m_btnMode,SIGNAL(clicked(bool)),this,SLOT(slot_changeImageMode()));
    connect(m_btnUpdate,SIGNAL(clicked(bool)),this,SLOT(slot_updateImages()));
    connect(m_imageListWid,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(slot_onListItemClick(QListWidgetItem*)));
}

void ThumbImageWidget::updateImageCount()
{
    m_imageCountText->setText(str_image_and_preview+"("+QString::number(m_imageListWid->count())+")");
}

void ThumbImageWidget::updateTipText()
{
    if(!editMode){
        m_tipText->setText("");
    }else{
        m_tipText->setText(str_imge_have_seleted1+QString::number(m_selectedItems.size())+str_imge_have_seleted2);
    }
}

void ThumbImageWidget::onImagesResInsert(QString path,QImage* thumb){
    QListWidgetItem *litsItem = new QListWidgetItem(/*QIcon(QPixmap::fromImage(m_images.at(i)).scaled(130,130)),NULL*/);
    litsItem->setSizeHint(QSize(thumb_image_width,thumb_image_width));

    ThumbImageItem *itemWid = new ThumbImageItem(path,thumb);
    m_imageListWid ->addItem(litsItem);
    m_imageListWid->setItemWidget(litsItem,itemWid);
    m_thumbs.insert(path,litsItem);
    updateImageCount();
}

void ThumbImageWidget::onImagesResRemove(QString path){
    if(m_thumbs.keys().contains(path)){
        QListWidgetItem *litsItem = m_thumbs[path];
        if(litsItem){
            m_imageListWid->removeItemWidget(litsItem);
            delete litsItem;
            mainWindow->slot_updateMedia();
        }
    }
}

void ThumbImageWidget::slot_onImagesResChanged()
{
    if(editMode){
        slot_changeImageMode();
    }

    updateImageCount();
    if(m_imageListWid->count()==0){
        emit m_middleWidgets->imageEmpty();
    }
}

void ThumbImageWidget::slot_onListItemClick(QListWidgetItem *listItem)
{
    ThumbImageItem *imageItem = (ThumbImageItem*)m_imageListWid->itemWidget(listItem);
    if(editMode){
        imageItem->onItemClick();

        if(imageItem->getCheckState()){
            m_selectedItems.append(imageItem);
        }else{
            m_selectedItems.removeOne(imageItem);
        }
        updateTipText();
    }
    else
    {
        emit m_middleWidgets->imageItemClick(imageItem->getImagePath(),new QImage(imageItem->getImagePath()));
    }
}

void ThumbImageWidget::slot_changeImageMode()
{
    if(editMode){
        for(int i=0;i<m_selectedItems.size();i++){
            m_selectedItems.at(i)->onItemClick();
        }
        m_selectedItems.clear();
        editMode = false;
        m_btnUpdate->setText(str_button_refresh_image);
        m_btnMode->setText(str_button_edit_image);
        updateTipText();
    }else{
        editMode = true;
        m_btnUpdate->setText(str_button_delete_image);
        m_btnMode->setText(str_button_edit_image_cancel);
        updateTipText();
    }
}

void ThumbImageWidget::slot_updateImages()
{
    if(m_selectedItems.size()>0&&editMode){
        int result = CMessageBox::showCMessageBox(this,str_question_delete_image,str_button_delete,str_button_cancel);
        if(result == RESULT_CONFIRM){
            // Delete images selected
            for(int i=0;i<m_selectedItems.size();i++){
                ThumbImageItem *imageItem = m_selectedItems.at(i);
                if(QFile::remove(imageItem->getImagePath())){
                    mainWindow->getGalleryWidget()->removeImage(imageItem->getImagePath());
                    emit m_middleWidgets->sig_imagesResRemove(imageItem->getImagePath());
                }
            }
            emit m_middleWidgets->imagesResChanged();
        }
    }else{
        mainWindow->slot_updateMedia();
    }
}
